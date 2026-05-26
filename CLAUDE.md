# CLAUDE.md

Guidance for working in this repository. For depth, see [`docs/`](./docs/README.md).

## What this is

**Jackal** is a compiler for a small custom language (`.jkl` files), written in C.
It lexes (flex) → parses (bison) → builds an AST → compiles to a linear,
stack-based **IR bytecode** intended for a separate VM, *jackalvm*. **The VM is
not in this repo** — this is the compiler + its support library `libjackal`.

Pipeline: `source.jkl → lexer → parser → AST → jkl_compile() → jkl_ir_t[] → (binary file)`.

It is a **working but incomplete prototype**: the front end and literal/arithmetic
codegen work and are unit-tested; control flow, functions, the symbol table, the
class system, the optimizer, and the evaluator are partial, stubbed, or
built-but-unused. Read [`docs/roadmap.md`](./docs/roadmap.md) before assuming a
feature works.

## Key files

- `jackal.c` — CLI entry (`./jackal file.jkl`); just opens the file and calls `yyparse()`.
- `jackal_lexer.l` — flex lexer **source** (generated → `jackal_lexer.c`, committed).
- `jackal_parser.y` — bison grammar **source**; its semantic actions build the AST
  and call `jkl_compile()` at EOF (generated → `jackal_parser.c` + `include/jackal_parser.h`, committed).
- `libjackal/jackal_compiler.c` — AST → IR code generator (the heart of the back end).
- `libjackal/jackal_ir.c`, `include/jackal/jackal_ir.h` — bytecode model & file I/O.
- `include/jackal/jackal_types.h` — base typedefs, the `struct jkl_node` AST, op enum.
- `libjackal/jackal_ast.c` — AST node new/append; `libjackal/jackal_context.c` — parse-time block stack.
- `tests/compiler.c` — Check-based tests asserting emitted IR.

> **Edit the `.l`/`.y` sources, never the generated `jackal_lexer.c` /
> `jackal_parser.c` / `include/jackal_parser.h`.** After editing them you need
> `flex`/`bison` to regenerate.

## Build / run / test

```sh
# Fresh build (generated files are NOT committed — bootstrap autotools first):
./autogen.sh && ./configure && make           # builds libjackal.a and the `jackal` binary
make CFLAGS="-g -DVERBOSE -DENABLE_COLOR"      # ...with diagnostic logging + colors

# Run the compiler on a sample (writes <basename>.bin, or a named output):
./jackal samples/main.jkl                      # -> main.bin (versioned bytecode; magic "JKLB")
./jackal samples/main.jkl out.bin

# Tests (auto-uses Check if present, else a -DJKL_NO_CHECK shim):
make check                                     # runs the unit + ASan integration suites
cd tests && make compiler                      # ...or run them standalone (no autotools)
```

Prereqs: `gcc make autoconf automake bison flex pkg-config` + the **Check**
library (`libcheck`) for tests (optional — there's a shim). Optional:
`clang-format`/`astyle` (formatting). See [`docs/build.md`](./docs/build.md).

## Conventions

- **Style**: 2-space indent, K&R/Linux braces, ~80-col. Code was formatted with
  astyle (`make -f tools.mk format`); `clang-format` is also available (no
  `.clang-format` committed).
- **Naming**: everything is `jkl_`-prefixed (`jkl_node_t`, `jkl_ir_t`,
  `jkl_compile`, …). Width typedefs: `jkl_byte/word/dword/qword/qqword_t`
  (note `jkl_word_t` is a 16-bit `short` and is used widely as the general int).
- **Errors**: `jkl_error(component, fmt, ...)` prints and **`exit(1)`** (also used
  as assertions). `jkl_warn/log/note` are no-ops unless built with `-DVERBOSE`.
- **AST**: one fat `struct jkl_node` with a `type` tag + a union of fields; only
  some fields are valid per type (table in [`docs/architecture.md`](./docs/architecture.md)).
  Only `BLOCK`/`PARAMS` nodes accept `jkl_node_append`.

## Gotchas (high-value to know up front)

- The CLI **emits a versioned bytecode file** (`jkl_ir_code_save`, format v1 with
  a `JKLB` header + ABI guard; `jkl_ir_code_load` is the inverse). `if`/`else`/
  `loop` jump targets are **correct** (backpatched; `JCP` = jump-if-false to an
  absolute instruction index, `JMP` = unconditional). See `docs/ir.md`.
- **Operator precedence is enforced** — operators are inlined into `expr` as
  terminals with a `%left`/`%nonassoc` ladder (`*` `/` `%` > `+` `-` > relational
  > `==` `!=` > `&&` > `||`; relational is non-associative). See `docs/language.md`.
- Teardown is centralized in `jkl_program_free` (recursive/type-aware
  `jkl_node_free` + symbol table + ir_code + the func/fixup lists); `main()` and the
  integration tests use it, and `./jackal samples/main.jkl` is ASan-clean. Build the
  ASan integration tests with `cd tests && make precedence`.
- **Variables resolve to slots**: `let`/`ID` go through `program->symbol_table`
  (a real `jkl_symbol_table_t`) — `ALLOC`/`STORE`/`LOAD` carry a 0-based slot
  index, not a name hash. Reading a name with no prior `let` is a compile error.
  (String *literals* still go into `bss` via a hash.)
- **Functions are lowered** (see `docs/ir.md` calling convention): top-level funcs
  are **hoisted after `HALT`** (entry = index 0); `CALL target,kind,nargs` is
  internal (`kind 0`, entry address) or external/builtin (`kind 1`, bss name, for
  undefined callees like `puts`); `return`→`RET`. Single-arg calls, flat slot table
  (no recursion/per-function scopes yet), calls are statements. `raise` is still a
  stub (hits the `default` error path).
- **Types are Ruby-like objects** (see `docs/types.md`): a tagged `jkl_value_t`
  (`jackal_value.h` — include it before `jackal_class.h`), `jkl_class` is the
  type/method registry, and `jkl_send(self, name, argv, argc)` is dynamic dispatch.
  `int`/`bool`/`string` have real native-C methods; `recv.method(args)` lowers to
  `SEND name_off,argc` and `true/false` to `PUSHB`. No VM runs `SEND` yet, so test
  in two tiers: **behavioral** via `jkl_send` (the non-ASan `compiler` target —
  `jkl_class_init` leaks its method tables, so keep it off ASan) and **structural**
  IR-shape via the ASan `precedence` target.
- **Arrays + iterators** (see `docs/arrays.md`): `[1,2,3]` literals lower to
  `NEWARR n`; `arr[i]` desugars to `SEND "at"`; iterators (`each`/`map`/`filter`/
  `reduce`) take a trailing block `{ |x| body-expr }` lowered to `PUSHBLK k`
  (per-program block table, in-process only). `jkl_ir_code_save` **refuses** to
  serialize a `.bin` containing `PUSHBLK` — blocks are evaluator-only in v1, run
  by `jkl_eval_expr`/`jkl_block_call` (the long-stubbed evaluator finally has a
  job). Memory rule: `jkl_array_t` keeps `items[]` as **borrowed views**, with
  a `trash[]` list holding owned heap payloads — `push` of an owned value MOVES
  it to trash, `at(i)` returns a view (don't `jkl_value_free` it). `jkl_value_dup`
  of an array is forbidden in v1.
- The **stack, optimizer, and evaluator are still built-but-unused** (or stubs);
  the `hash` module is no longer used by the compiler.
- **Parser regen trap**: if you edit `jackal_parser.y`, regenerate with `bison`
  AND copy the fresh `jackal_parser.h` over `include/jackal_parser.h` (the path
  the lexer's `#include <jackal_parser.h>` resolves to). The committed copies must
  match, or the old token set silently shadows the new one.
- Build is bootstrapped via `./autogen.sh`; `make check` runs the tests (a
  `check-local` hook drives the standalone `tests/Makefile`, which still works on
  its own). `tools.mk` paths now point at `libjackal/`; the gperf keyword table is
  deleted (was dead code).
- Generated **source** is committed (`jackal_lexer.c`, `jackal_parser.c`,
  `include/jackal_parser.h`) — don't hand-edit them. Generated **build** files
  (`Makefile`, `config.h`, `libjackal/Makefile`) are NOT committed (gitignored;
  produced by `configure`), so a fresh clone must run `./autogen.sh`.

## Where to read more

`docs/README.md` (index) · `architecture.md` (pipeline) · `language.md` (the
`.jkl` language) · `ir.md` (bytecode) · `types.md` (type system + methods) ·
`modules.md` (per-file API + status) · `build.md` (setup) · `roadmap.md` (what
works / known bugs / next steps).
