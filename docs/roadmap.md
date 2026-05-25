# Status, Known Issues & Roadmap

This is the honest assessment of what works, what is half-built, and what is
scaffolding. It consolidates the issues called out throughout the other docs.

Legend: ✅ works · 🟡 partial / buggy · 🟥 stub / unwired · ⬛ planned-only.

## Feature maturity matrix

### Front end (lexer + parser)

| Feature | Status | Notes |
|---------|:------:|-------|
| Tokenizing keywords/operators/punctuation | ✅ | `jackal_lexer.l` |
| Int / float / string literals (+ escapes) | ✅ | float requires a `.`; UTF-8 in strings OK |
| Block comments `/* */` | 🟡 | no line comments, no nesting, no EOF-in-comment guard; comment chars may ECHO |
| Identifiers | ✅ | ASCII only |
| `let` declaration | ✅ | only binding form |
| Binary expressions | ✅ | precedence + associativity enforced (operators inlined as terminals with a `%left`/`%nonassoc` ladder) |
| Parenthesized expressions | ✅ | use to force grouping |
| `if` / `elif` / `else` | ✅ | full conditional; `elif` desugars to `else { if … }` |
| `loop` | ✅ (parse) | infinite loop only; no `break`/`while`/`for` |
| `call` (`puts "x"`, `f x`) | ✅ | one arg, no parens; callee kept on the AST (`call->id`) |
| `func` definition | ✅ | parsed and appended to the program block |
| `return expr` | ✅ (parse) | expr is mandatory |
| `raise "msg"` | ✅ (parse) | string only |
| Unary `!` | 🟥 | tokenized, in `jkl_op_t`, but no grammar rule |
| Assignment statement (`x := 5`) | ⬛ | `JKL_NODE_ASSIGNMENT` enum only |
| gperf keyword table | 🟥 | generated but not used by the lexer |

### Back end (compiler → IR)

| Feature | Status | Notes |
|---------|:------:|-------|
| AST → IR for literals | ✅ | `PUSHI`/`PUSHF`/`LOAD` |
| Binary operators → IR | ✅ | post-order, all ops mapped |
| `let` → `ALLOC/…/STORE` | ✅ | unit-tested |
| `loop` lowering | ✅ | back-edge backpatched to first body instruction; unit-tested |
| `if` / `if-else` lowering | ✅ | `JCP`/`JMP` backpatched; defined contract; unit-tested |
| `call` lowering | ✅ | `CALL target,kind,nargs`; internal (entry addr) / external (bss name); single arg |
| `func` lowering | 🟡 | hoisted after `HALT`; param prologue + `RET`; flat slots, no recursion |
| `return` lowering | ✅ | `<expr>` then `RET` |
| `raise` lowering | 🟥 | hits the `default` error path |
| Variable *reads* resolve to slots | ✅ | `let`/`ID` resolve to symbol-table slots; undeclared reads error |
| IR serialization to file | ✅ | versioned format (header + ABI guard); save **and** load; round-trip tested |
| CLI emits a bytecode file | ✅ | `./jackal in.jkl [out.bin]` writes the `.bin` |
| Optimizer | 🟥 | type-check only, never invoked |
| Evaluator / interpreter (`USE_EVAL`) | 🟥 | stub; disabled |

### Supporting modules

| Module | Status | Notes |
|--------|:------:|-------|
| types, error, string, ast, context, ir | ✅ | the working core |
| hash | 🟡 | works for non-colliding keys; `free` fixed; no longer used by the compiler |
| symbol_table | ✅ | wired: `program->symbol_table` resolves `let`/`ID` to slots |
| stack | ✅/🟥 | correct, but unused |
| class (OOP runtime) | 🟥 | full model + built-ins declared; all methods `jkl_not_implemented`; `jkl_class_init` never called |

## Consolidated known bugs

Correctness bugs that would bite if the relevant path were exercised:

1. ✅ *Fixed.* `if`/`if-else` now backpatch `JCP`/`JMP` to correct forward
   targets (with a defined jump contract). `libjackal/jackal_compiler.c`.
2. ✅ *Fixed.* `loop` back-edge now targets the first body instruction
   (still infinite by design — no `break`). `libjackal/jackal_compiler.c`.
3. ✅ *Fixed.* `statement: func` now appends the node, so top-level functions enter
   the AST and are compiled. `jackal_parser.y`.
4. ✅ *Fixed.* `call` keeps the callee on `call->id`; the compiler resolves it to an
   internal or external `CALL`. `jackal_parser.y`, `libjackal/jackal_compiler.c`.
5. ✅ *Fixed.* `let`/`ID` now resolve to symbol-table slots
   (`ALLOC`/`STORE`/`LOAD <slot>`); a read of an undeclared name is a compile
   error. `libjackal/jackal_compiler.c`.
6. ✅ *Fixed.* `jkl_hash_free` now runs the cleanup loop before zeroing
   `capacity`, then frees the buckets. `libjackal/jackal_hash.c`.
7. ✅ *Fixed.* `jkl_symbol_table_free` frees the whole heap block (struct +
   embedded array) instead of the embedded array. `libjackal/jackal_symbol_table.c`.
8. **Hash map / `bss` string store have no collision handling** — colliding keys
   overwrite. `libjackal/jackal_hash.c:55`, `libjackal/jackal_ir.c:82`.
9. ✅ *Fixed.* Operators are inlined into `expr` as terminals with a `%left`/
   `%nonassoc` precedence ladder, so `1 + 2 * 3` groups as `1 + (2*3)` and same-
   precedence operators are left-associative. `jackal_parser.y`.
10. **`JKL_ARITY` macros read the wrong byte** (low instead of high) and are
    unused. `include/jackal/jackal_ir.h:69`.
11. ✅ *Fixed.* `jkl_node_free` is now recursive and type-aware (frees owned
    children, the `compound.nodes` array, and lexer-allocated strings); `main()`
    tears down the AST + symbol table. `libjackal/jackal_ast.c`, `jackal.c`.
12. **Comment scanner can ECHO** comment bytes (exclusive `CMT` state has no
    catch-all rule). `jackal_lexer.l:62`.

Build/tooling issues (see [`build.md`](./build.md)):

13. No `configure` checked in (need `autoreconf -i`); no `autogen.sh`.
14. Tests still not wired into `make check`, but `tests/Makefile` now uses the
    correct `libjackal/` path, compiles the library sources directly, and falls
    back to a Check-free harness (`-DJKL_NO_CHECK`, `tests/no_check.h`) when
    `libcheck` is absent. (`tools.mk` still has the old `lib/` glob.)
15. gperf keyword generation not integrated into the autotools build.
16. `LICENSE.txt` still has the template copyright line; `AUTHORS`/`README`/
    `ChangeLog`/`NEWS` are empty.

## Where the project is heading

From the git history, the arc has been:

- hand-written Makefiles → **migrated to autotools** (`2dc0ed2`);
- AST + error/debug tooling → **IR module introduced** (`33a0240`) → stack/hash
  objects (`c8e1427`) → **Check-based tests** (`a46f8ac`);
- recent commits: move the program to the heap, dependency-install targets, path
  reconfiguration.

So the trajectory is: a front end that's basically done, a **young IR back end**
being filled in construct-by-construct, and a large amount of forward-looking
runtime scaffolding (class system, stack, symbol table, evaluator) waiting to be
connected — presumably once the companion **jackalvm** exists to execute the
emitted bytecode.

## Suggested next steps

A pragmatic order for making the compiler end-to-end useful:

1. ✅ *Done.* The CLI emits a versioned bytecode file (`jkl_ir_code_save` +
   `jkl_ir_code_load`, header/ABI guard — see [`ir.md`](./ir.md)).
2. ✅ *Done.* Control-flow targets corrected (`if`/`else` + `loop` backpatching),
   with a documented `JCP`/`JMP` contract and unit tests.
3. ✅ *Done.* The symbol table is wired: `let`/`ID` resolve to numbered slots via
   `jkl_symbol_table_t`; undeclared reads error. Covered by unit + ASan tests.
4. ✅ *Done.* `call`/`func`/`return` lowered: top-level funcs compiled, callee kept
   and resolved (internal/external `CALL`), params bound, `RET` emitted, with a
   documented calling convention (see [`ir.md`](./ir.md)). Single-arg, flat slots,
   no recursion yet; `raise` still pending.
5. **Repair the build glue** — `autogen.sh`, integrate tests into `make check`,
   fix the stale `lib/` paths, and (optionally) wire gperf in or delete it.
6. ✅ *Done.* Memory bugs (#6 hash free, #7 symbol-table free, #11 recursive
   AST free + teardown) fixed and operator precedence (#9) enforced; covered by
   the ASan integration tests (`tests/precedence.c`, `cd tests && make precedence`).
