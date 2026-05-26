# Arrays and Iterators

Arrays are heterogeneous, heap-allocated containers of `jkl_value_t`. They use
the same registry + `jkl_send` dispatch as the other primitive types
(see [`types.md`](./types.md)). This document defines the memory model, the v1
method set, and the block/iterator surface.

## Source syntax

```jkl
let a := [1, 2, 3]              # array literal
let n := a.length               # methods via the regular . dispatch
let x := a[1]                   # sugar for a.at(1)

# iterators take a block as the trailing argument:
let r1 := a.map    { |x| x.succ }
let r2 := a.filter { |x| x.odd? }
let s  := a.reduce(0) { |acc, x| acc + x }
let _  := a.each   { |x| puts x.to_s }    # each returns self
```

Calls are expressions and bind tighter than every operator (postfix on `term`),
so `a[0] + 1` is `(a[0]) + 1` and `a.map { … }.length` chains.

## Memory model — the trash-list rule

A `jkl_array_t` (see `include/jackal/jackal_array.h`) holds:
- `items[]` — every element here is a **borrowed view** (`owned = 0`). Reading is
  cheap; `jkl_array_get` returns the view directly. **A caller of `at(i)` must
  never `jkl_value_free` what they got back.**
- `trash[]` — owned heap payloads that the array must release at drop time.

`jkl_array_push(arr, v)` moves `v` into `trash[]` when `v.owned` and `v` carries
a heap payload (string/array/object), then stores a *borrowed view* of the same
buffer at `items[len]`. The caller's `v` is "spent" (ownership transferred,
matching the existing `concat`/`upcase` convention). Scalars and borrowed
strings store as-is — nothing trashed.

`jkl_array_free(arr)` walks `trash[]`, calls `jkl_value_free` on each entry, then
frees `items`/`trash`/the container. `jkl_value_free` on a `JKL_T_ARRAY` value
with `owned = 1` calls `jkl_array_free`.

**`jkl_value_dup` of an array is forbidden in v1** (errors). Each array has
exactly one owner. This eliminates the aliasing/double-free trap without GC or
refcounting, and degrades gracefully to refcounting later if sharing is needed.

### Why this works
- `arr.push("literal")` — the string literal is borrowed from `bss`; nothing is
  moved to trash; the literal lives forever; safe.
- `arr.push("abc".upcase)` — `upcase` returns an owned string; it's moved into
  `trash`; `items[]` holds a borrowed view; when `arr` is freed, the owned copy
  is released.
- `arr.map { |x| x.upcase }` — each block result is owned; `push` trashes each;
  the result array owns its contents.
- `arr.at(0)` — returns the borrowed view; safe to read, never freeable.
- `arr.pop` — if the popped element had a corresponding trash entry, ownership
  is transferred back to the returned value (the caller now owns it).

## Implemented methods (v1)

| Method | Notes |
|---|---|
| `length` | `Integer` |
| `at(i)` | borrowed view of `items[i]` (do not free) |
| `push(x)` | trashes owned payloads; returns self |
| `pop` | returns the last element (transfers ownership if trashed) |
| `clear` | drops items + frees trash; returns self |
| `empty?` | `Boolean` |
| `reverse` | returns a **new** array; strings are deep-copied (nested arrays/blocks would error via the forbidden-`dup` rule) |
| `join(sep)` | returns an owned `String` |
| `each(blk)` | applies the block to each element; returns self |
| `map(blk)` | collects block results into a new array |
| `filter(blk)` | new array of elements where the block returns truthy (the result borrows from the source — keep the source alive) |
| `reduce(init, blk)` | accumulator pattern; the native impl frees the previous accumulator before replacing it |

**Deferred** (kept as `jkl_not_implemented`): `new`, `insert`, `remove`, `sort`,
`shift`, `unshift`, `[]=` assignment. `insert`/`remove` break the simple trash
model; `sort` needs a comparator block; the rest are mechanical follow-ups.

## Blocks (lambdas, the evaluator-only construct)

A block is a small AST + an env: `{ |params| body-expr }`. v1 constraints:
- **Body is exactly one expression** (the block's result value).
- **No closures** — only the named params are in scope. Outer-variable capture
  is a deferred milestone.
- Max 8 params (`JKL_BLOCK_MAX_PARAMS`).
- Param names need not be `?`/`!`-suffixed; method calls inside the body use the
  same `jkl_send` registry as everywhere else.

The block runtime value (`JKL_T_BLOCK`) carries non-owning pointers into the
program's AST: `jkl_block_t { jkl_node_t *params; jkl_node_t *expr; }`. Block
values are `owned = 0`; `jkl_value_free` on a block is a no-op (the program owns
the AST). The `jkl_block_t` headers themselves are tracked per-program in
`program->blocks[]` and freed by `jkl_program_free`.

### Invocation

Native iterators (and any C code) invoke a block with
`jkl_value_t jkl_block_call(jkl_value_t block, jkl_value_t *argv, jkl_word_t argc)`
(in `jackal_eval.h`). It binds `argv` to the formal params and runs the body
through `jkl_eval_expr` — a small tree-walking expression evaluator (`jackal_eval.c`)
that handles literals, identifier lookup in the env, binops (via
`jkl_apply_binop` in `jackal_ops.c` — shared with the future VM), method calls
(via `jkl_send`), and nested array/block literals.

## IR

Two new opcodes:

| Opcode | args | Meaning |
|---|---|---|
| `JKL_IR_NEWARR` (`0x01e9`) | `[n]` | Pop `n` values, push a new array containing them (left-to-right). |
| `JKL_IR_PUSHBLK` (`0x01ea`) | `[k]` | Push a block value from `program->blocks[k]` (in-process only). |

**Method-call lowering** is unchanged in shape — the trailing block is just the
last positional arg: compile receiver, compile any paren args left-to-right,
then `PUSHBLK k` (if there's a block), then `SEND name_off, argc`. Iterator
contracts therefore are: `each(blk)` → argv = `[blk]`; `reduce(init, blk)` →
argv = `[init, blk]`. Indexing `arr[i]` desugars at parse time to a
`JKL_NODE_METHOD_CALL` for `at`, so it lowers to a regular `SEND` — no
dedicated opcode.

**Serialization (the honesty rule):** `jkl_ir_code_save` scans the instruction
stream for `PUSHBLK` and refuses to write a `.bin` if it finds one (clear error:
"blocks/iterators are evaluator-only in v1"). Programs that don't use blocks
serialize as before. This keeps the file format honest about what runs end-to-end
without a VM.

## Testing (two tiers, as for other primitives)

- **Behavioral** (`tests/compiler.c`, non-ASan `compiler` target): hand-build
  arrays via `jkl_array_new`/`jkl_array_push`; dispatch through `jkl_send`. For
  iterators, build the block AST in C (a `JKL_NODE_BLOCK_LIT`), wrap as a
  `JKL_T_BLOCK` value, and pass to `each`/`map`/`reduce` — this exercises the
  full chain `jkl_send → native method → jkl_block_call → jkl_eval_expr → jkl_send`.
- **Structural** (`tests/precedence.c`, ASan `precedence` target): parse real
  source and assert the emitted IR shape — `[1,2,3]` → `NEWARR 3`, `a[i]` →
  `SEND "at"`, `arr.each { |x| x.succ }` → `PUSHBLK + SEND` with the right
  ordering and `program->n_blocks == 1`. Memory-clean teardown via
  `jkl_program_free`.

## Roadmap

Future additive work, in rough order of payoff:
- **Closures** (capture outer slots in block values) — needed for blocks that
  read outer variables.
- **Multi-statement block bodies** — currently restricted to a single expression.
- **Reference semantics for arrays** (refcount or GC) — would re-enable
  `jkl_value_dup` and let arrays be passed/aliased freely.
- `[]=` assignment, `insert`/`remove`/`sort`, `shift`/`unshift`.
- A dedicated `JKL_IR_INDEX` opcode (today indexing pays a name lookup).
- **Serializable blocks** in the bytecode file (with the in-process block table
  written into a new file section).
- **Hashes** (`{key: value}` literals + `Hash` class).
- The **VM**, which is the thing that finally executes `SEND`/`NEWARR`
  end-to-end.
