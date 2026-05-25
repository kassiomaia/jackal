# Module Reference

A file-by-file tour of the compiler library (`libjackal/`) and the public headers
(`include/jackal/`). Each module is tagged with its role in the pipeline:

- **WIRED** — actively used by the lexer/parser/compiler path.
- **PARSE-ONLY** — used during parsing but not by the back end.
- **UNUSED** — compiled into `libjackal` but not referenced by the pipeline
  (available via the API / for future work).
- **STUB** — present but does essentially nothing yet.

For status/known-bugs see [`roadmap.md`](./roadmap.md); for the pipeline overview
see [`architecture.md`](./architecture.md).

## At a glance

| Module | Role | Header | Impl | Notes |
|--------|------|--------|------|-------|
| types | WIRED | `jackal_types.h` | — | Base typedefs, AST node struct, op enum |
| error | WIRED | `jackal_error.h` | (header-only) | `jkl_error/warn/log/note`, colors, `jkl_debug` |
| string | WIRED | `jackal_string.h` | `jackal_string.c` | `strlen` wrapper + DJB2 hash |
| ast | WIRED | `jackal_ast.h` | `jackal_ast.c` | Node new/free/append/print |
| context | PARSE-ONLY | `jackal_context.h` | `jackal_context.c` | Parse-time block/scope stack |
| ir | WIRED | `jackal_ir.h` | `jackal_ir.c` | Opcodes, buffer, string store, save |
| compiler | WIRED | `jackal_compiler.h` | `jackal_compiler.c` | AST → IR; owns `jkl_program_t` |
| hash | UNUSED* | `jackal_hash.h` | `jackal_hash.c` | Generic hash map (*allocated as `symbol_table`, never read) |
| symbol_table | UNUSED | `jackal_symbol_table.h` | `jackal_symbol_table.c` | Fixed-size symbol array |
| stack | UNUSED | `jackal_stack.h` | `jackal_stack.c` | Typed LIFO stack |
| class | UNUSED | `jackal_class.h` | `jackal_class.c` | OOP class/method model + built-ins |
| optimizer | UNUSED/STUB | `jackal_optimizer.h` | `jackal_optimizer.c` | Type-check on binop, no optimization |
| eval | STUB | `jackal_eval.h` | `jackal_eval.c` | Interpreter placeholder |
| keywords | UNUSED | `jackal_keywords.h` | (generated) | gperf table, not called by lexer |

Umbrella header `include/jackal.h` pulls in all of the above (plus libc and,
oddly, `sys/socket.h`/`netinet/in.h` — networking headers that nothing uses).

---

## WIRED modules

### types — `include/jackal/jackal_types.h`

Foundational typedefs and the AST model. No `.c` file.

- **Width typedefs**: `jkl_byte_t`=`char`, `jkl_word_t`=`short`,
  `jkl_dword_t`=`int`, `jkl_qword_t`=`long`, `jkl_qqword_t`=`long long`, plus
  `jkl_float_t/double_t/ldouble_t`, `jkl_string_t`=`char*`, `jkl_bool_t`,
  `jkl_char_t`, `jkl_any_t`=`void*`, `jkl_function_t`.
  > Note: most code uses `jkl_word_t` (a 16-bit `short`) as its general
  > integer/return type, which bounds several sizes/counters to 16 bits.
- **`jkl_node_type_t`** — the 17 AST node kinds.
- **`jkl_op_t`** — the 15 operators (incl. `JKL_OP_NOT`, which the grammar can't
  produce).
- **`struct jkl_node`** — the single fat AST node (see
  [`architecture.md`](./architecture.md#how-the-ast-is-shaped)).

### error — `include/jackal/jackal_error.h`

Header-only diagnostics. ANSI color macros gated by `ENABLE_COLOR`; verbosity
gated by `VERBOSE`.

- `jkl_error(component, fmt, ...)` — print to stderr and **`exit(1)`** (fatal,
  no recovery). Used pervasively as both error reporting and assertion.
- `jkl_warn / jkl_log / jkl_note(...)` — only emit when compiled with `-DVERBOSE`;
  otherwise compiled out.
- `jkl_debug(action)` — a tiny interactive prompt (continue / run action / quit)
  for ad-hoc debugging.

### string — `include/jackal/jackal_string.h`, `libjackal/jackal_string.c`

Two functions: `jkl_string_len` (a `strlen` wrapper) and `jkl_string_hash` (DJB2,
returns `abs(hash)`). The hash is the workhorse for variable/string identity in
codegen and the data section. Complete and correct.

### ast — `include/jackal/jackal_ast.h`, `libjackal/jackal_ast.c`

- `jkl_node_new(type)` — allocate and zero a node.
- `jkl_node_free(node)` — free a single node (**non-recursive**; the tree leaks).
- `jkl_node_append(parent, child)` — append to a `BLOCK`/`PARAMS` node
  (`realloc` one slot at a time); calls `exit(1)` for other node types.
- `jkl_print_ast_node/type` — debug printers (only `ID`/`STRING`/`BINOP` print
  detail).

### ir — `include/jackal/jackal_ir.h`, `libjackal/jackal_ir.c`

The bytecode model and buffer. Fully covered in [`ir.md`](./ir.md). Public funcs:
`jkl_ir_code_init`, `jkl_ir_code_push`, `jkl_ir_code_save`, `jkl_ir_code_free`,
`jkl_ir_store_string`.

### compiler — `include/jackal/jackal_compiler.h`, `libjackal/jackal_compiler.c`

Owns the program object and the AST→IR walk.

```c
typedef struct {
  jkl_node_t   *ast_prog_root;   // root BLOCK
  jkl_error_t   n_errors;
  jkl_word_t    n_warnings, n_notes, n_ids, n_funcs;
  jkl_hash_tbl *symbol_table;    // allocated by the parser, never used
  jkl_ir_code_t*ir_code;         // the emitted bytecode
} jkl_program_t;
```

- `jkl_program_new` / `jkl_program_init` — allocate program + a 1024-instruction
  IR buffer.
- `jkl_compile(program)` — walk `ast_prog_root`, emit IR, append `HALT`.
- internal: `jkl_compile_block`, `jkl_compile_expr`, `jkl_emit_expr_op`,
  `jkl_get_idx` (the variable counter helper — itself never called).

See [`ir.md`](./ir.md#how-constructs-are-lowered) for what each node emits and the
known codegen bugs.

## PARSE-ONLY modules

### context — `include/jackal/jackal_context.h`, `libjackal/jackal_context.c`

A file-scope global stack (`frames[1024]`) tracking the current block while the
parser builds the tree: `jkl_push_context`, `jkl_pop_context`, `jkl_get_context`,
`jkl_ensure_empty_contexts`. The `program` argument is ignored (global state →
one program per process). Contains a dead unused global `jkl_node_t *cc`. See
[`architecture.md`](./architecture.md#the-parse-time-context-stack).

## UNUSED / STUB modules

These compile and (mostly) work in isolation, but nothing in the active pipeline
calls them. They represent intended future runtime/semantic features.

### hash — `include/jackal/jackal_hash.h`, `libjackal/jackal_hash.c`

A generic string-keyed hash map with typed values (`STRING/INT/FLOAT/BOOL/ARRAY/
OBJECT/NIL`) and a `jkl_hash_to_json` serializer. The parser allocates one as
`program->symbol_table` but never `set`s or `get`s it.

- **Direct-index, no collision resolution**: `set`/`get` index `hash % capacity`
  with no probing/chaining — colliding keys overwrite.
- **Bug**: `jkl_hash_free` zeroes `capacity` *before* its cleanup loop
  (`jackal_hash.c:35`), so the loop body never runs and nested `OBJECT` values
  leak.

### symbol_table — `include/jackal/jackal_symbol_table.h`, `libjackal/jackal_symbol_table.c`

A fixed `jkl_symbol_t[1024]` with symbol kinds (`LET/FUNCTION/CLASS/
CLASS_METHOD`), `add` (linear, rejects duplicates) and `get` (linear search).
Never instantiated by the pipeline.

- **Bug**: `jkl_symbol_table_free` calls `free(table->symbols)` on an array that
  is embedded in the struct, not separately allocated (`jackal_symbol_table.c:52`).

### stack — `include/jackal/jackal_stack.h`, `libjackal/jackal_stack.c`

A correct, typed LIFO stack (`bp/sp/capacity/items`) with
`stack_push/pop/peek/size/free` and typed-push macros
(`stack_pushc/i/f/s`). Fixed capacity (errors on overflow). Not used anywhere —
likely intended for the VM or constant-folding.

### class — `include/jackal/jackal_class.h`, `libjackal/jackal_class.c`

The largest utility (~430 lines): a full **OOP class/object model** for the
Jackal *runtime* (not a compiler-internal abstraction). Supports single
inheritance (`base`), mixins/includes, a method table with flags
(`STATIC/INSTANCE` + override variants), class flags
(`STANDARD/ABSTRACT/FINAL/NATIVE`), `is_a` / `is_base_of` / `is_included_in`
queries, a global class registry, and `jkl_class_init()` that declares built-in
classes (`Object`, `String`, `Integer`, `Float`, `Boolean`, `Array`, `IO`) with
rich method signatures.

- **All built-in methods point at `jkl_not_implemented()`** — the shape exists,
  the behavior does not.
- `jkl_class_init()` is **never called**; no parser/compiler code references
  classes. This is forward-looking scaffolding for a runtime that does not exist
  yet.

### optimizer — `include/jackal/jackal_optimizer.h`, `libjackal/jackal_optimizer.c`

`jkl_optimize_expr(node)` inspects a binop and, if both sides are primitive
literals, errors on type mismatch. It performs **no optimization** (no constant
folding, DCE, etc.) and is never called.

### eval — `include/jackal/jackal_eval.h`, `libjackal/jackal_eval.c`

`jkl_eval(program)` is the would-be tree-walking interpreter (alternative to
`jkl_compile`, behind the disabled `USE_EVAL`). It only runs invariant checks and
returns 0 — a placeholder.

### keywords — `include/jackal/jackal_keywords.h` (generated from `defs/keywords.gperf`)

A gperf perfect-hash `keyword_lookup()` with a per-keyword `has_block` flag. Not
called by the lexer (which hardcodes keywords), so it is dead code today.

## Vendored / generated, not project modules

- `include/check.h`, `include/check_stdint.h` — the **Check** C unit-test
  framework, vendored (LGPL). Used by `tests/`.
- `jackal_lexer.c`, `jackal_parser.c`, `include/jackal_parser.h` — generated by
  flex/bison and committed in-tree. Edit the `.l`/`.y` sources, not these.
- `config.h` — autotools-generated package metadata.
- `tags` — a committed ctags index.
