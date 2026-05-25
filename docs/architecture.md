# Architecture

This document explains how Jackal turns a `.jkl` source file into IR bytecode,
and how the modules wire together. For the language itself see
[`language.md`](./language.md); for the bytecode see [`ir.md`](./ir.md); for a
per-module API reference see [`modules.md`](./modules.md).

## The pipeline

```
 ┌──────────┐   tokens   ┌──────────┐   builds   ┌─────────┐  walks   ┌──────────┐  emits  ┌────────────┐
 │  Lexer   │ ─────────► │  Parser  │ ─────────► │   AST   │ ───────► │ Compiler │ ──────► │ IR bytecode│
 │ (flex)   │            │ (bison)  │            │ jkl_node│          │          │         │ jkl_ir_t[] │
 └──────────┘            └──────────┘            └─────────┘          └──────────┘         └────────────┘
 jackal_lexer.l          jackal_parser.y         jackal_types.h       jackal_compiler.c    jackal_ir.{c,h}
```

1. **Lexer** (`jackal_lexer.l`, generated → `jackal_lexer.c`). A flex scanner
   turns characters into tokens (`LET`, `ID`, `CINT`, `PLUS`, …). String and
   block-comment handling use flex exclusive start-conditions (`%x STR`,
   `%x CMT`). See [`language.md`](./language.md#lexical-structure).

2. **Parser** (`jackal_parser.y`, generated → `jackal_parser.c` +
   `include/jackal_parser.h`). A bison LALR parser whose semantic actions build
   the AST directly (there are no separate per-node builder functions — each rule
   calls `jkl_node_new()` and fills fields). See the
   [grammar](./language.md#grammar).

3. **AST** (`struct jkl_node` in `include/jackal/jackal_types.h:78`). A single
   "fat" node struct: a `type` tag plus a union of fields, only some of which are
   meaningful for any given node type. Built/owned via `libjackal/jackal_ast.c`.

4. **Compiler** (`libjackal/jackal_compiler.c`). Recursively walks the AST root
   (a `JKL_NODE_BLOCK`) and emits IR instructions into a growable buffer.

5. **IR** (`libjackal/jackal_ir.c`, `include/jackal/jackal_ir.h`). The bytecode
   model: a fixed-size instruction record plus a code buffer with an embedded
   data section (`bss`). Can be serialized to a binary file.

## Entry point and control flow

The CLI (`jackal.c`) drives the pipeline:

```c
yyin = fopen(in, "r");
yyparse();                    // lexer+parser build the AST (sets the global `program`)
fclose(yyin);
jkl_compile(program);         // AST ──► IR
jkl_ir_code_save(program->ir_code, out);   // IR ──► versioned .bin file
```

The AST is built **inside the parser**. The grammar's top-level `program` rule
(`jackal_parser.y`) creates the program object up front and finalizes the AST at
end of input:

```c
program: { /* prologue action */
            program = jkl_program_new();                 // alloc jkl_program_t
            program->symbol_table = jkl_hash_new();      // (created, then never used)
            program->ast_prog_root = jkl_node_new(JKL_NODE_BLOCK);
            jkl_push_context(program, program->ast_prog_root);
         }
       | program statements { /* epilogue action, runs at EOF */
            jkl_pop_context(program);
            jkl_ensure_empty_contexts();
         }
       ;
```

So the **parser→compiler hand-off** is: the parser builds the AST and leaves it
on the global `program`; `main()` then calls `jkl_compile(program)` (AST → IR)
and `jkl_ir_code_save()` (IR → file). (Compilation used to be invoked from inside
the grammar epilogue, which also `free`d `program`; it now lives in `main` so the
emitted `ir_code` can be saved and freed cleanly.)

> The output file uses a versioned format (`JKLB` header). See
> [`ir.md`](./ir.md#serialization-file-format-v1). An alternative interpreter
> path (`USE_EVAL` → `jkl_eval`) remains a disabled stub.

`jkl_eval(program)` (`libjackal/jackal_eval.c`) is an alternative
tree-walking-interpreter entry point, but it is an unimplemented stub and is not
wired into the CLI.

## The parse-time context stack

Because the grammar builds a tree while reading a flat token stream, it needs to
know "which block am I currently appending statements to?". That is the job of
the **context stack** in `libjackal/jackal_context.c`:

- A single global fixed-size stack (`context.frames[1024]`, `context.pos`).
- `jkl_push_context(program, node)` — enter a new block/params scope (also sets
  `node->parent`).
- `jkl_pop_context(program)` — leave the current scope, returning the node.
- `jkl_get_context(program)` — the current scope (top of stack), or `NULL`.
- `jkl_ensure_empty_contexts()` — asserts the stack is empty after parsing.

Every compound construct (the program root, `loop`, `if`, `func` body, and
function `params`) pushes a fresh `JKL_NODE_BLOCK`/`JKL_NODE_PARAMS`, lets its
inner statements append themselves to the current context via
`jkl_node_append()`, then pops. Statement rules (`let`, `raise`, `call`,
`return`) append directly to `jkl_get_context(program)`.

> The `program` argument to these functions is ignored — the stack is a file-
> scope global, so only one program can be parsed per process. There is also a
> dead unused global `jkl_node_t *cc;` (`jackal_context.c:5`).

## How the AST is shaped

`struct jkl_node` (`include/jackal/jackal_types.h:78`) overloads many pointer
fields; which ones are live depends on `type`:

| Node type | Live fields |
|-----------|-------------|
| `JKL_NODE_INT` / `FLOAT` / `STRING` | `value.i` / `value.f` / `value.s` |
| `JKL_NODE_ID` | `value.s` (the name) |
| `JKL_NODE_LET` | `id` (a `JKL_NODE_ID`), `expr` |
| `JKL_NODE_BINOP` | `binop.left`, `binop.op`, `binop.right` |
| `JKL_NODE_BLOCK` / `PARAMS` | `compound.nodes[]`, `compound.n_nodes` |
| `JKL_NODE_IF` | `expr` (condition), `block` |
| `JKL_NODE_LOOP` | `block` |
| `JKL_NODE_CALL` | `node` (the argument: a string or ident) |
| `JKL_NODE_RETURN` | `expr` |
| `JKL_NODE_RAISE` | `value.s` |
| `JKL_NODE_FUNC` | `id`, `params`, `block` |
| `JKL_NODE_PARAM` | `id` |

Only `JKL_NODE_BLOCK` and `JKL_NODE_PARAMS` accept children via
`jkl_node_append()` (`jackal_ast.c:123`); appending to anything else calls
`exit(1)`. The child array is grown with `realloc` one element at a time (O(n²)),
which is fine at these sizes.

`JKL_NODE_ASSIGNMENT` exists in the enum but is never produced by the parser, and
`jkl_node_free` does not recurse — the whole AST is leaked at process exit (the
OS reclaims it). See [`roadmap.md`](./roadmap.md).

## Module dependency overview

```
                       jackal.h  (umbrella include)
                           │
   ┌───────────────┬───────┴─────────┬───────────────┬──────────────┐
 jackal_types    jackal_error     jackal_string    jackal_ast    jackal_ir
 (typedefs,      (jkl_error/      (len, DJB2       (node new/     (opcodes,
  AST struct,     warn/log/note    hash)            append,        encoding,
  op enum)        macros)                           print)         buffer, save)
                           │                            │              │
                           ▼                            ▼              ▼
                     jackal_context (parse-time)   jackal_compiler  (AST ──► IR)
                                                         │
       built but NOT wired into the pipeline:           │ creates/uses
       jackal_stack, jackal_symbol_table,        ◄──────┘ ir_code (+ symbol_table, unused)
       jackal_class, jackal_optimizer, jackal_eval, jackal_hash*

   * jackal_hash IS used once (the parser allocates program->symbol_table) but
     it is never populated or read.
```

Actively-wired modules: **types, error, string, ast, context, ir, compiler**.
Built-but-unused / stub modules: **stack, symbol_table, class, optimizer, eval**,
and effectively **hash** (allocated but unused). See [`modules.md`](./modules.md)
for the per-module detail and [`roadmap.md`](./roadmap.md) for status.
