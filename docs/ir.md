# Intermediate Representation (IR) / Bytecode

The compiler's output is a linear, stack-based bytecode: the **IR**. It is meant
to be executed by a separate VM (*jackalvm*, not in this repo). Defined in
`include/jackal/jackal_ir.h` and `libjackal/jackal_ir.c`.

This document supersedes the older [`jackal_ir.md`](./jackal_ir.md) and
[`jackal_compiler.md`](./jackal_compiler.md) design notes.

## Instruction set

All opcodes are in `jkl_ir_type_t` (`include/jackal/jackal_ir.h:10`). The
"arity" column is the number of immediate arguments the opcode is intended to
carry (see [encoding](#opcode-encoding)); stack-only operations take their
operands from the value stack.

| Opcode | Hex | Arity | Meaning |
|--------|-----|:-----:|---------|
| `JKL_IR_NOPE`  | `0x00ca` | 0 | No-op / buffer filler |
| `JKL_IR_ALLOC` | `0x01cb` | 1 | Allocate storage for a variable |
| `JKL_IR_LOAD`  | `0x01cc` | 1 | Load a value (from the data section / a variable) |
| `JKL_IR_STORE` | `0x01cd` | 1 | Store top-of-stack into a variable |
| `JKL_IR_PUSHI` | `0x01ce` | 1 | Push an integer immediate |
| `JKL_IR_PUSHF` | `0x01cf` | 1 | Push a float immediate |
| `JKL_IR_PUSHS` | `0x01d0` | 1 | Push a string |
| `JKL_IR_POPI`  | `0x01d1` | 1 | Pop integer |
| `JKL_IR_POPF`  | `0x01d2` | 1 | Pop float |
| `JKL_IR_POPS`  | `0x01d3` | 1 | Pop string |
| `JKL_IR_ADD`   | `0x00d4` | 0 | `a + b` |
| `JKL_IR_SUB`   | `0x00d5` | 0 | `a - b` |
| `JKL_IR_MUL`   | `0x00d6` | 0 | `a * b` |
| `JKL_IR_DIV`   | `0x00d7` | 0 | `a / b` |
| `JKL_IR_MOD`   | `0x00d8` | 0 | `a % b` |
| `JKL_IR_EQL`   | `0x00d9` | 0 | `a == b` |
| `JKL_IR_NEQ`   | `0x00da` | 0 | `a != b` |
| `JKL_IR_GT`    | `0x00db` | 0 | `a > b` |
| `JKL_IR_GTE`   | `0x00dc` | 0 | `a >= b` |
| `JKL_IR_LT`    | `0x00dd` | 0 | `a < b` |
| `JKL_IR_LTE`   | `0x00de` | 0 | `a <= b` |
| `JKL_IR_AND`   | `0x00df` | 0 | `a && b` |
| `JKL_IR_OR`    | `0x00e0` | 0 | `a \|\| b` |
| `JKL_IR_NOT`   | `0x00e1` | 0 | `!a` |
| `JKL_IR_JMP`   | `0x01e2` | 1 | Unconditional jump |
| `JKL_IR_JCP`   | `0x01e3` | 1 | Conditional jump |
| `JKL_IR_CALL`  | `0x01e4` | 1 | Call |
| `JKL_IR_RET`   | `0x00e5` | 0 | Return |
| `JKL_IR_HALT`  | `0x00e6` | 0 | Halt execution |

## Opcode encoding

The opcode value packs the intended arity into its **high byte**, and the macros
read arity from the **low byte** mask. Concretely
(`include/jackal/jackal_ir.h:69`):

```c
#define JKL_ARITY(type)        (type & 0x00ff)
#define JKL_IS_ARITY(type, n)  (JKL_ARITY(type) == n)
#define JKL_IS_ARITY_0(type)   JKL_IS_ARITY(type, 0)
#define JKL_IS_ARITY_1(type)   JKL_IS_ARITY(type, 1)
#define JKL_IS_ARITY_2(type)   JKL_IS_ARITY(type, 2)
```

Observed convention in the table above: opcodes whose **high byte is `0x01`**
take one immediate argument (`ALLOC`, `LOAD`, `STORE`, `PUSH*`, `POP*`, `JMP`,
`JCP`, `CALL`); opcodes whose high byte is `0x00` are zero-argument stack/control
ops. The low byte is a sequential id (`0xca`..`0xe6`).

> Caveat: the `JKL_ARITY`/`JKL_IS_ARITY_*` macros key off the **low** byte, not
> the high byte, so they do not actually return the arity shown above, and they
> are **never used** anywhere in the code. Treat the high byte as the real arity
> indicator. This mismatch is a known cleanup item ([`roadmap.md`](./roadmap.md)).

## Instruction & buffer layout

```c
#define JKL_IR_ARG_COUNT 3

typedef struct {
  jkl_ir_type_t type;                  // 4 bytes (enum/int)
  jkl_qqword_t  args[JKL_IR_ARG_COUNT];// 3 × 8 bytes
} jkl_ir_t;                            // => 28 bytes + padding per instruction
```

Every instruction is the same fixed size regardless of arity; unused argument
slots are 0. Emitting is done with the helper macro
`JKL_EMIT_IR(type, a, b, c)`.

```c
typedef struct {
  jkl_ir_t      *ir;     // growable array of instructions
  jkl_qqword_t   n_irs;  // number of instructions emitted
  jkl_byte_t     bss[(1 << (sizeof(jkl_word_t) * 8)) - 1];  // 65535-byte data section
} jkl_ir_code_t;
```

- `ir` / `n_irs` — the instruction stream.
- `bss` — a fixed **64 KB-ish data section** (65535 bytes) where string/identifier
  bytes are placed (see [strings](#strings-and-the-data-section)).

### Buffer management (`libjackal/jackal_ir.c`)

- `jkl_ir_code_init(code, size)` — allocate (min 1024 instructions), fill with
  `NOPE`, zero the start of `bss`.
- `jkl_ir_code_push(code, ir)` — append one instruction, growing the array by
  1024 whenever `n_irs` hits a multiple of 1024; returns the new instruction's
  index (useful as a jump target). This is the function the compiler uses.
- `jkl_ir_code_free(code)` — free the instruction array.

There are also header macros `JKL_IR_CODE_ALLOC/PUSH/STORE/FREE` that implement a
parallel (slightly different) scheme; the compiler path uses the
`jkl_ir_code_*` **functions**, not these macros.

## Strings and the data section

`jkl_ir_store_string(code, str)` (`libjackal/jackal_ir.c:73`) places a string's
bytes into `bss` at `position = djb2_hash(str) % 65535` and the compiler emits a
`LOAD` whose argument is that hash, so the VM can find the bytes again.

> This is hash-placement **without collision handling**: two strings that hash to
> nearby offsets can overwrite each other, and the stored span can run past the
> hashed slot. It works for the small test cases but is not robust. See
> [`roadmap.md`](./roadmap.md).

## Serialization (file format v1)

`jkl_ir_code_save(code, filename)` writes a **versioned** binary file: a packed
32-byte header, then the instruction stream, then the data section.

```
off size field           type / value
0   4    magic           char[4] = "JKLB"
4   2    version         jkl_word_t  = 1  (JKL_IR_FORMAT_VERSION)
6   1    endianness      jkl_byte_t  (0 = little, 1 = big; the writing host's)
7   1    ir_struct_size  jkl_byte_t  = sizeof(jkl_ir_t)   (ABI guard, e.g. 32)
8   8    n_irs           jkl_qqword_t  (instruction count)
16  8    bss_len         jkl_qqword_t  (data-section length, 65535 today)
24  8    reserved        jkl_qqword_t  = 0  (future: entry point / flags)
─── then n_irs × sizeof(jkl_ir_t) bytes: the jkl_ir_t[] stream, verbatim
─── then bss_len bytes: the data section, verbatim
```

The header struct (`jkl_ir_file_header_t`) and the `JKL_IR_MAGIC*` /
`JKL_IR_FORMAT_VERSION` macros live in `include/jackal/jackal_ir.h` and are shared
by writer and reader.

`jkl_ir_code_load(code, filename)` is the inverse: it reads the header, validates
`magic`, `version`, `ir_struct_size == sizeof(jkl_ir_t)`, and `endianness ==`
host, then reads the instructions and data section. **v1 rejects** a file written
on a different-endian or different-ABI host rather than silently misreading it;
cross-endian byteswapping is a future (v2) additive change keyed off the
`endianness` byte.

The standalone `jackal` binary now **does** write this file: `./jackal in.jkl
[out.bin]` compiles and saves, defaulting the output name to the input basename
with a `.bin` extension. (`.gitignore` lists `ir.bin`/`ir.code`.)

## How constructs are lowered

The code generator is `libjackal/jackal_compiler.c`. Expression lowering
(`jkl_compile_expr`) is post-order so operands are on the stack before the
operator:

| AST | Emitted IR |
|-----|-----------|
| `JKL_NODE_INT` | `PUSHI value` |
| `JKL_NODE_FLOAT` | `PUSHF value` |
| `JKL_NODE_STRING` | store string in `bss`, then `LOAD hash, hash+len` |
| `JKL_NODE_ID` | store name in `bss`, then `LOAD hash` |
| `JKL_NODE_BINOP` | `<left>`, `<right>`, then the operator opcode |

Statement lowering (`jkl_compile_block`):

| AST | Emitted IR |
|-----|-----------|
| `JKL_NODE_LET` | `ALLOC hash(name)`, `<expr>`, `STORE hash(name)` |
| `JKL_NODE_IF` (no else) | `<cond>`, `JCP end`, `<then>` — `JCP` backpatched to the index after `<then>` |
| `JKL_NODE_IF` (with else) | `<cond>`, `JCP else`, `<then>`, `JMP end`, `<else>` — both jumps backpatched |
| `JKL_NODE_LOOP` | `<body>`, `JMP start` (back-edge to the first body instruction) |
| `JKL_NODE_CALL` | `CALL 0` *(callee/arg ignored)* |
| `JKL_NODE_FUNC` | (`n_funcs++`), `<block>` inlined *(no prologue/epilogue/linkage)* |
| `JKL_NODE_RETURN` | nothing (warning only) |

`jkl_compile` finishes by emitting a single `HALT`
(`libjackal/jackal_compiler.c:227`).

### Worked example — `let x := 42`

```
ALLOC  hash("x")
PUSHI  42
STORE  hash("x")
HALT
```

(This is exactly what `tests/compiler.c::test_jkl_compile_check_let_with_int`
asserts.)

### Worked example — `let x := "value"`

```
ALLOC  hash("x")
LOAD   hash("value"), hash+len      # value bytes were placed in bss
STORE  hash("x")
HALT
```

### Worked example — `loop { let x := "value" }`

```
0: ALLOC  hash("x")
1: LOAD   hash("value"), hash+len
2: STORE  hash("x")
3: JMP    0                      # back-edge to the first body instruction
4: HALT
```

### Worked example — `if x==1 { puts "a" } else { puts "b" }`

```
0: LOAD  hash("x")
1: PUSHI 1
2: EQL
3: JCP   6        # condition false -> jump to else
4: CALL  0        # then: puts "a"
5: JMP   7        # skip the else
6: CALL  0        # else: puts "b"
7: HALT
```

`elif` is desugared by the parser into `else { if ... }`, so it reuses this exact
shape recursively (no dedicated `elif` opcodes).

## Jump semantics (the contract)

Because the VM does not exist yet, these definitions **are** the spec:

- Jump targets are **absolute instruction indices** (into the `jkl_ir_t[]`
  stream).
- `JMP target` — unconditional: set `PC = target`.
- `JCP target` — pop the top of stack; if it is **zero / false**, set
  `PC = target`; otherwise fall through. (I.e. "jump over the then-block.")

Targets are filled in by **backpatching**: the compiler emits the jump with a
placeholder, compiles the block, then rewrites the target via
`jkl_ir_code_patch()` (`jkl_ir_code_push` returns the index to patch).

## Known IR/codegen issues

Fixed in this version: `if`/`else` and `loop` jump targets are now correct
(backpatched), the `JCP`/`JMP` contract is defined above, and serialization is
versioned (header + ABI guard). Remaining gaps, tracked in
[`roadmap.md`](./roadmap.md):

- **`ID` is lowered like a string** (hash + `LOAD` from `bss`) rather than being
  resolved through the symbol table, so variable *reads* don't reference the slot
  that `ALLOC`/`STORE` created. (`jackal_compiler.c`)
- **`CALL` discards the callee and arguments** (always `CALL 0`).
- **`RETURN` and `RAISE` are not lowered** (`RETURN` is a no-op; `RAISE` would
  hit the `default` error in `jkl_compile_block`).
- **`loop` has no exit** — it is infinite by language design (no `break`).
- **Endianness/ABI** — the file records the host's endianness and
  `sizeof(jkl_ir_t)`; the loader currently *rejects* mismatches rather than
  converting them.
