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
| Binary expressions | 🟡 | parse fine, but **precedence is not enforced** (operator is a non-terminal) |
| Parenthesized expressions | ✅ | use to force grouping |
| `if` / `elif` / `else` | ✅ | full conditional; `elif` desugars to `else { if … }` |
| `loop` | ✅ (parse) | infinite loop only; no `break`/`while`/`for` |
| `call` (`puts "x"`, `f x`) | 🟡 | one arg, no parens; **callee name discarded** in AST |
| `func` definition | 🟡 | parsed, but **top-level `func` is not appended to the program block** |
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
| `call` lowering | 🟥 | emits `CALL 0`; callee/args ignored |
| `func` lowering | 🟥 | body inlined; no prologue/epilogue/linkage/params |
| `return` lowering | 🟥 | no-op (warning only) |
| `raise` lowering | 🟥 | hits the `default` error path |
| Variable *reads* resolve to slots | 🟥 | `ID` is hashed+`LOAD`ed like a string, not via symbol table |
| IR serialization to file | ✅ | versioned format (header + ABI guard); save **and** load; round-trip tested |
| CLI emits a bytecode file | ✅ | `./jackal in.jkl [out.bin]` writes the `.bin` |
| Optimizer | 🟥 | type-check only, never invoked |
| Evaluator / interpreter (`USE_EVAL`) | 🟥 | stub; disabled |

### Supporting modules

| Module | Status | Notes |
|--------|:------:|-------|
| types, error, string, ast, context, ir | ✅ | the working core |
| hash | 🟡/🟥 | works for non-colliding keys; **`free` bug**; allocated as `symbol_table` but never read |
| symbol_table | 🟥 | not wired; **invalid `free` of embedded array** |
| stack | ✅/🟥 | correct, but unused |
| class (OOP runtime) | 🟥 | full model + built-ins declared; all methods `jkl_not_implemented`; `jkl_class_init` never called |

## Consolidated known bugs

Correctness bugs that would bite if the relevant path were exercised:

1. ✅ *Fixed.* `if`/`if-else` now backpatch `JCP`/`JMP` to correct forward
   targets (with a defined jump contract). `libjackal/jackal_compiler.c`.
2. ✅ *Fixed.* `loop` back-edge now targets the first body instruction
   (still infinite by design — no `break`). `libjackal/jackal_compiler.c`.
3. **`func` definitions are dropped.** The `statement: func` rule has no action to
   append the node, so top-level functions never enter the AST.
   `jackal_parser.y:158` / `:301`.
4. **`call` loses the callee.** Only the single argument is stored on the
   `JKL_NODE_CALL`; the function name (`ID`) is discarded. `jackal_parser.y:279`.
5. **Variable reads don't reference their storage.** `ID` lowering hashes the name
   into `bss` and `LOAD`s that, instead of resolving the `ALLOC`/`STORE` slot.
   `libjackal/jackal_compiler.c:139`.
6. **`jkl_hash_free` never frees nested data.** `capacity` is zeroed before the
   cleanup loop, so the loop never runs → leak. `libjackal/jackal_hash.c:35`.
7. **`jkl_symbol_table_free` invalid free.** `free(table->symbols)` on an embedded
   array. `libjackal/jackal_symbol_table.c:52`.
8. **Hash map / `bss` string store have no collision handling** — colliding keys
   overwrite. `libjackal/jackal_hash.c:55`, `libjackal/jackal_ir.c:82`.
9. **Operator precedence not enforced** — `op` is a non-terminal so `%left`
   declarations don't apply to `expr op expr`; the grammar is ambiguous.
   `jackal_parser.y:71`/`:161`.
10. **`JKL_ARITY` macros read the wrong byte** (low instead of high) and are
    unused. `include/jackal/jackal_ir.h:69`.
11. **AST is never freed** — `jkl_node_free` is non-recursive and the tree is
    leaked at exit. `libjackal/jackal_ast.c:28`.
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
3. **Wire up the symbol table** — resolve `ID` reads/writes to allocated slots
   instead of hashing names into `bss`; this unlocks real variables.
4. **Finish `call`/`func`/`return`** — keep the callee, pass arguments, give
   functions linkage, and emit `RET`.
5. **Repair the build glue** — `autogen.sh`, integrate tests into `make check`,
   fix the stale `lib/` paths, and (optionally) wire gperf in or delete it.
6. **Address the memory bugs** (#6, #7, #11) and grammar precedence (#9).
