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
# Fresh build (NO configure is checked in — bootstrap autotools first):
autoreconf -i && ./configure && make          # builds libjackal.a and the `jackal` binary
make CFLAGS="-g -DVERBOSE -DENABLE_COLOR"      # ...with diagnostic logging + colors

# Run the compiler on a sample (writes <basename>.bin, or a named output):
./jackal samples/main.jkl                      # -> main.bin (versioned bytecode; magic "JKLB")
./jackal samples/main.jkl out.bin

# Tests (auto-uses Check if present, else a -DJKL_NO_CHECK shim; NOT in `make check`):
cd tests && make compiler
```

Prereqs: `gcc make autoconf automake bison flex pkg-config` + the **Check**
library (`libcheck`) for tests. Optional: `gperf` (for the unused keyword
header), `clang-format`/`astyle` (formatting). See [`docs/build.md`](./docs/build.md).

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
- The AST is freed at exit: `jkl_node_free` is **recursive/type-aware** (frees
  owned children + lexer strings); `main()` also frees the symbol table. Build the
  ASan integration tests with `cd tests && make precedence`.
- **Variables resolve to slots**: `let`/`ID` go through `program->symbol_table`
  (a real `jkl_symbol_table_t`) — `ALLOC`/`STORE`/`LOAD` carry a 0-based slot
  index, not a name hash. Reading a name with no prior `let` is a compile error.
  (String *literals* still go into `bss` via a hash.)
- Still incomplete: `call`/`func`/`return`/`raise` codegen is stubbed or wrong.
  Because of the `call`/`func` stubs, `./jackal samples/main.jkl` still leaks a few
  token strings (the discarded callee + dropped top-level `func`) — not a regression.
- The **stack, class system, optimizer, and evaluator are built-but-unused** (or
  stubs). Don't assume they participate in compilation. (The symbol table is now
  wired; the `hash` module is no longer used by the compiler.)
- **Parser regen trap**: if you edit `jackal_parser.y`, regenerate with `bison`
  AND copy the fresh `jackal_parser.h` over `include/jackal_parser.h` (the path
  the lexer's `#include <jackal_parser.h>` resolves to). The committed copies must
  match, or the old token set silently shadows the new one.
- `tools.mk` still references the old `lib/` directory (renamed to `libjackal/`);
  the gperf keyword header is dead code. (`tests/Makefile` is fixed.)
- Generated files (`jackal_lexer.c`, `jackal_parser.c`), `config.h`, `tags`, and
  the vendored `include/check.h` are committed — don't hand-edit them.

## Where to read more

`docs/README.md` (index) · `architecture.md` (pipeline) · `language.md` (the
`.jkl` language) · `ir.md` (bytecode) · `modules.md` (per-file API + status) ·
`build.md` (setup) · `roadmap.md` (what works / known bugs / next steps).
