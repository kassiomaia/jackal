# Building, Running, and Testing

Jackal uses **GNU Autotools** for the main build (`configure.ac`, `Makefile.am`,
`libjackal/Makefile.am`), with some legacy helper targets in `tools.mk`. This
document covers prerequisites, a fresh build, running the compiler, the tests,
and the editor/formatting tooling — and is explicit about what currently blocks a
clean build.

## Prerequisites

| Tool | Why | Required for |
|------|-----|--------------|
| `gcc` (C compiler) | compiles C | building everything |
| `make` | drives the build | building everything |
| `autoconf`/`automake`/`autoreconf` | generate `configure` & `Makefile`s | **fresh build** (no `configure` is checked in) |
| `bison` (yacc) | parser generator | regenerating `jackal_parser.c` |
| `flex` (lex) | lexer generator | regenerating `jackal_lexer.c` |
| `gperf` | keyword table generator | only the (currently unused) keyword header |
| `pkg-config` | finds libraries | tests (locating `check`) |
| **Check** (`libcheck`) | C unit-test framework | building/running the tests |
| `clang-format` *or* `astyle` | formatting | optional |

> The generated lexer/parser **are committed in-tree** (`jackal_lexer.c`,
> `jackal_parser.c`, `include/jackal_parser.h`), so in principle you don't need
> `flex`/`bison` just to compile — but `Makefile.am` lists them as
> `BUILT_SOURCES`, so a from-scratch `make` may try to regenerate them. Have
> `flex` and `bison` installed to avoid surprises.

### Installing prerequisites

Debian/Ubuntu:

```sh
sudo apt-get install build-essential autoconf automake flex bison gperf pkg-config check clang-format
```

`tools.mk` also has `make install_deps` (→ `install_astyle`, `install_cppcheck`)
which downloads and builds **astyle** and **cppcheck** from source into `./tools`.
These targets are for the formatting/lint tooling only and are unrelated to the
autotools build. (They predate the `lib/` → `libjackal/` rename and may need a
look — see [legacy `tools.mk`](#legacy-toolsmk).)

## Fresh build

No `configure` script is committed (it's git-ignored), so you must bootstrap
autotools first:

```sh
autoreconf -i          # generate configure, Makefile.in, config.h.in, aux scripts
./configure
make
```

This builds:

- `libjackal/libjackal.a` — the static compiler library (`noinst_LIBRARIES`).
- `jackal` — the CLI binary (`bin_PROGRAMS`), linked against `-ljackal -lm`.

Build knobs (from `Makefile.am`): `AM_YFLAGS=-d` (bison emits the header);
include path is `-I$(top_srcdir)/include`.

To get the diagnostic logging (`jkl_log/warn/note`) and colors, build with the
verbose/color flags, e.g.:

```sh
make CFLAGS="-g -DVERBOSE -DENABLE_COLOR"
```

(Without `-DVERBOSE`, those macros compile to nothing; only fatal `jkl_error`
output remains.)

## Running the compiler

```sh
./jackal samples/main.jkl
```

The CLI (`jackal.c`) opens the file and calls `yyparse()`, which parses and then
runs `jkl_compile()` on the AST.

> Important: the standalone binary does **not** currently write an output file —
> it parses and compiles in-memory and exits. With `-DVERBOSE` you'll see the
> lexer/parser/compiler logs (and `no rules implemented for ...` warnings for the
> unfinished node types). Emitting a bytecode file (`jkl_ir_code_save`) is only
> done via the library API today. See
> [`architecture.md`](./architecture.md#entry-point-and-control-flow) and
> [`roadmap.md`](./roadmap.md).

## Tests

Tests use the **Check** framework and live in `tests/` (`main.c` runs the suite
defined in `compiler.c`). They construct ASTs directly and assert the emitted IR
opcode sequence (e.g. `let x := 42` → `ALLOC, PUSHI, STORE, HALT`). Six tests
cover program init, empty compile, and `let`(int/string) / `loop` codegen.

The tests are **not** integrated with `make`/`make check` (there is no `TESTS`
variable and `tests/` is not in `SUBDIRS`). They have their own `tests/Makefile`:

```sh
cd tests
make compiler        # builds ../bin/jackaltest, then runs it
```

> Caveat: `tests/Makefile` still references the **pre-rename** library path
> (`-L../lib`, `make -C ../lib`) and outputs to `../bin/jackaltest`. After the
> autotools migration the library lives in `libjackal/`, so these paths are
> stale and the test Makefile needs updating (and `libcheck` must be installed).
> See [`roadmap.md`](./roadmap.md).

## Formatting & linting (`tools.mk`)

- **Format**: `make -f tools.mk format` runs **astyle** with a Linux-style,
  2-space config over `lib/*.c` and `include/**/*.h`. The committed code is
  formatted this way (2-space indent, braces, `--max-code-length=80`). Note the
  `lib/*.c` glob is stale (now `libjackal/`). `clang-format` is also available as
  an alternative, though there is no `.clang-format` in the repo.
- **Lint**: `make -f tools.mk check` runs **cppcheck** over `lib/*.c` (same stale
  glob).
- These are convenience targets, separate from the autotools build.

## Editor tooling

- `compile_flags.txt` → clangd: `-xc -Iinclude`.
- `.vscode/settings.json` → disables IntelliSense squiggles, associates
  `jackal_compiler.h` as C.
- `.vscode/tasks.json` → a generic "build active file" task using `gcc-11`
  (not a project build; prefer the autotools build above).
- `tags` → a committed ctags index (regenerate with `ctags -R` if you have
  ctags).

## Project metadata

- **Package**: `jackal` **1.0** (`configure.ac`, `config.h`).
- **License**: MIT (`LICENSE.txt`) — note the copyright line is still the
  template `Copyright (c) [year] [fullname]`.
- `AUTHORS`, `README`, `ChangeLog`, `NEWS` exist but are **empty**.
- `COPYING`/`INSTALL` are symlinks into the system automake install.

## Known fresh-build blockers (summary)

1. **No `configure` checked in** — must run `autoreconf -i` first. There is no
   `autogen.sh`/`bootstrap.sh` helper.
2. **`flex` / Check may be missing** — install them (and `bison`, `gperf`) before
   building/testing.
3. **Tests are not wired into autotools** and `tests/Makefile` points at the old
   `lib/`/`bin/` paths.
4. **`gperf` keyword generation is not in the autotools build** (it's an orphaned
   `tools.mk` rule); harmless because the generated header is unused.

See [`roadmap.md`](./roadmap.md) for the consolidated issue list and suggested
fixes.
