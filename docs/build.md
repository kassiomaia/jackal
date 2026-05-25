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
./jackal samples/main.jkl              # writes main.bin (input basename + .bin)
./jackal samples/main.jkl out.bin      # or name the output explicitly
```

The CLI (`jackal.c`) parses the file, runs `jkl_compile()`, and writes the IR to
a versioned bytecode file (see [`ir.md`](./ir.md#serialization-file-format-v1)).
You can eyeball the header with `od -A d -t x1 -N 32 main.bin` — it begins with
the ASCII magic `JKLB`.

> With `-DVERBOSE` you'll also see lexer/parser/compiler logs on stderr. Note the
> lexer echoes unmatched newlines to stdout (a known cosmetic quirk; see
> [`language.md`](./language.md)).

## Tests

Tests use the **Check** framework and live in `tests/` (`main.c` runs the suite
defined in `compiler.c`). They construct ASTs directly and assert the emitted IR
opcode sequence (e.g. `let x := 42` → `ALLOC, PUSHI, STORE, HALT`). Six tests
cover program init, empty compile, and `let`(int/string) / `loop` codegen.

The tests are **not** integrated with `make`/`make check` (there is no `TESTS`
variable and `tests/` is not in `SUBDIRS`). They have their own `tests/Makefile`:

```sh
cd tests
make compiler        # unit tests: build the lib sources + tests, then run them
make precedence      # integration tests: drive the real front end, under ASan
```

`tests/Makefile` compiles the `libjackal/` sources directly (no autotools build
needed) and auto-detects `libcheck` via `pkg-config`: if present it links
`-lcheck`; otherwise it builds with `-DJKL_NO_CHECK` and uses the dependency-free
shim in `tests/no_check.h`. So the suite runs even without `libcheck` installed.
The unit tests assert emitted IR opcode **sequences and jump targets**, plus a
save→load round-trip. They are not yet wired into `make check`.

The `precedence` target (`tests/precedence.c`) links the committed generated
parser/lexer (`jackal_parser.c`, `jackal_lexer.c`) and parses real source under
**AddressSanitizer** (`-fsanitize=address`): it verifies operator precedence /
associativity end-to-end and frees each parsed AST, so leaks or invalid frees in
`jkl_node_free` / the container frees fail the run. Needs `bison` only if you
changed the grammar (the generated sources are committed); `flex` is not required.

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
2. **`flex` only needed to regenerate the lexer** — the committed `jackal_lexer.c`
   lets `make` build without `flex`. `bison` is needed only if you change
   `jackal_parser.y`. The tests no longer require `libcheck` (see above).
3. **Stale parser header trap** — if you edit `jackal_parser.y` and regenerate,
   the fresh `jackal_parser.h` must also overwrite `include/jackal_parser.h`
   (the path the lexer's `#include <jackal_parser.h>` resolves to via
   `-Iinclude`); otherwise the old token set shadows it. See [`roadmap.md`](./roadmap.md).
4. **`gperf` keyword generation is not in the autotools build** (it's an orphaned
   `tools.mk` rule); harmless because the generated header is unused.

See [`roadmap.md`](./roadmap.md) for the consolidated issue list and suggested
fixes.
