# The Type System (primitives, methods, dispatch)

Jackal models values the way Ruby does: every value is an instance of a **class**,
and operations on it are **methods** looked up on that class. This document is the
architecture for the type system and the guide for extending it.

> Status: the runtime core (value model + registry + dispatch) and the primitive
> method sets for `int`/`bool`/`string` are implemented and unit-tested; the
> frontend lowers `.`-method calls and `true`/`false` to bytecode. Execution of
> that bytecode is the future VM's job — see "What runs where".

## The three layers

```
  source            x.upcase / true / "a".concat(b)
    │  (frontend: lexer + grammar)
    ▼
  AST               JKL_NODE_METHOD_CALL { node=receiver, id=name, params=args }
    │  (compiler: jkl_compile_expr)         JKL_NODE_BOOL
    ▼
  IR bytecode       … receiver, args …, SEND name_off, argc      PUSHB 0/1
    │  (future VM)
    ▼
  runtime           jkl_send(receiver_value, "name", argv, argc)  → jkl_value_t
```

## Runtime value model — `include/jackal/jackal_value.h`

A value is a small tagged struct (the representation the VM and every native
method share):

```c
typedef enum { JKL_T_NIL, JKL_T_INT, JKL_T_FLOAT, JKL_T_BOOL,
               JKL_T_STRING, JKL_T_ARRAY, JKL_T_OBJECT } jkl_type_tag_t;

typedef struct {
  jkl_type_tag_t tag;
  jkl_bool_t     owned;   /* STRING/ARRAY/OBJECT: free the payload on drop */
  union { jkl_qqword_t i; jkl_float_t f; jkl_string_t s; jkl_any_t obj; } as;
} jkl_value_t;
```

Constructors `jkl_int/jkl_float/jkl_bool/jkl_string` (borrowed) /
`jkl_string_owned` (takes ownership) / `jkl_nil`. **Ownership rule:** literals are
borrowed (`owned = 0`); methods that build new strings (`upcase`, `concat`, …)
return `jkl_string_owned`. `jkl_value_free` frees only owned payloads;
`jkl_value_dup` deep-copies a string; `jkl_value_to_cstr` returns owned memory.
(Bool reuses the `i` slot as 0/1.)

## Registry + dispatch — `jackal_class.{h,c}`

Classes/methods reuse the existing class model:

- `jkl_class_t` — `name`, `base` (single inheritance), `includes[]` (mixins),
  `methods[]`, `flags`. Built-ins live in a global table; `jkl_class_init()`
  creates `Object/String/Integer/Float/Boolean/Array/IO`.
- `jkl_method_t` — `name`, `arity`, `flags`, and a native C function:
  ```c
  typedef jkl_value_t (*jkl_native_fn_t)(jkl_value_t self, jkl_value_t *argv,
                                         jkl_word_t argc);
  ```
- **tag → class map**: populated at the end of `jkl_class_init`, so a value's tag
  selects its class in O(1). `jkl_class_for_tag(tag)`.
- **resolution**: `jkl_class_method_lookup(klass, name)` walks `self → includes →
  base`.
- **dispatch (the core primitive)**:
  ```c
  jkl_value_t jkl_send(jkl_value_t self, jkl_string_t name,
                       jkl_value_t *argv, jkl_word_t argc);
  ```
  tag → class → lookup → call. An unknown method is a `jkl_error`
  ("undefined method 'x' for <Class>"). `jkl_send` is what the VM will run for the
  `SEND` opcode, and what the unit tests call directly today.

## IR: `SEND` and `PUSHB`

- `PUSHB 0|1` — push a boolean.
- `SEND name_off, argc` — `name_off` is the `bss` offset of the NUL-terminated
  method name; `argc` is the argument count. **Stack discipline:** the compiler
  emits the receiver, then args left-to-right, so at `SEND` the stack is
  `… receiver arg0 … arg_{argc-1}`. VM contract: pop `argc` args + the receiver,
  call `jkl_send(receiver, name, argv, argc)`, push the result. (See
  [`ir.md`](./ir.md).)

## Language surface

```jkl
let s := "hello"
let n := s.length          # no-arg method
let u := s.upcase
let c := "ab".concat("cd") # method with args
let b := true              # bool literal
```

Method calls are **expressions** and bind tighter than every operator (they are
postfix on `term`), so `s.length + 1` is `(s.length) + 1` and `a.b.c` chains.

## Implemented methods

| Class | Methods |
|-------|---------|
| String | `length`, `upcase`, `downcase`, `reverse`, `concat(s)`, `empty?`, `to_i` |
| Integer | `to_s`, `abs`, `succ`, `pred`, `even?`, `odd?` |
| Boolean | `to_s`, `not` |

`?`/`!`-suffixed names are valid registry keys but the lexer's identifier charset
excludes `?`/`!`, so `even?`/`empty?` are reachable via `jkl_send` (and the VM) but
not yet from source — a frontend follow-up.

## How to add a method

1. Implement a `jkl_native_fn_t` in `libjackal/jackal_methods.c` (declare it in
   `include/jackal/jackal_methods.h`), e.g.:
   ```c
   jkl_value_t jkl_str_upcase(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
   { ... return jkl_string_owned(result); }
   ```
2. Register it in `jkl_class_init` on the right class:
   ```c
   jkl_class_add_method(String,
       jkl_method_new("upcase", 0, JKL_METHOD_INSTANCE, jkl_str_upcase));
   ```
3. Add a behavioral test (`tests/compiler.c`, via `jkl_send`) and, if it's callable
   from source, an IR-shape test (`tests/precedence.c`).

That's it — no IR, grammar, or compiler change is needed for a new method on an
existing type; `SEND` dispatches by name.

## What runs where (no VM yet)

There is no VM, so `SEND` bytecode is produced but **not executed** end-to-end.
Correctness is verified in two tiers:
- **behavioral** — call `jkl_send` directly and assert the returned value
  (`tests/compiler.c`, run on the non-ASan `compiler` target because
  `jkl_class_destroy_all` doesn't free method tables).
- **structural** — assert the emitted IR shape (`tests/precedence.c`, ASan).

## Roadmap

- Arrays & hashes (heap container values for `JKL_T_ARRAY`/`OBJECT`).
- `new`/instances and user-defined classes/methods from source.
- Blocks/iterators (`times`, `each`, `map`, `reduce`).
- Operators as methods (unify `+`/`==` with dispatch instead of dedicated opcodes).
- `?`/`!` in identifiers; method `flags` (static vs instance) semantics.
- A dedicated method-name section in the bytecode (the current `bss` interning has
  no collision handling — shared with string literals).
- The **VM** (`jackalvm`) that executes `SEND`/`PUSHB` against `jkl_send`.
