# Jackal — Documentation Index

Jackal is a compiler for a small, custom programming language (source files use
the `.jkl` extension). It is written in C and follows a classic compiler
front-/back-end split:

```
  source.jkl ──► lexer (flex) ──► parser (bison) ──► AST ──► compiler ──► IR bytecode ──► (binary file)
                                                                                            │
                                                                                            ▼
                                                                                   jackalvm  (separate
                                                                                   VM, NOT in this repo)
```

The compiler produces a linear, stack-based **Intermediate Representation (IR)**
— a bytecode that is intended to be executed by a separate virtual machine
called *jackalvm*. The VM itself does not live in this repository; this repo is
**only the compiler** plus its supporting library `libjackal`.

> Status in one line: a **working but incomplete prototype**. The lexer, parser,
> AST, and the literal/arithmetic parts of code generation work and are
> unit-tested. Control flow, functions, the symbol table, the class/object
> system, the optimizer, and the evaluator are partially implemented, stubbed,
> or built-but-unused. See [`roadmap.md`](./roadmap.md) for the full maturity
> matrix.

## Documents

| Document | What it covers |
|----------|----------------|
| [`architecture.md`](./architecture.md) | End-to-end pipeline, how the modules wire together, the parser→compiler hand-off, the parse-time context stack. |
| [`language.md`](./language.md) | The Jackal language: lexical rules, full grammar, every construct with examples, semantics, and what the syntax does/doesn't support. |
| [`ir.md`](./ir.md) | The IR/bytecode: opcode table, the arity-in-low-byte encoding, instruction & code-buffer layout, the binary file format, and per-construct codegen examples. |
| [`types.md`](./types.md) | The type system: the tagged value model, the class/method registry + `jkl_send` dispatch, the `SEND` opcode, the `int`/`bool`/`string` method tables, and how to add a method. |
| [`modules.md`](./modules.md) | File-by-file reference of `libjackal` and the headers, with each module's API and maturity (wired-in / built-but-unused / stub). |
| [`build.md`](./build.md) | Prerequisites, how to build / run / test, the formatting/lint tooling, and the concrete blockers for a fresh build. |
| [`roadmap.md`](./roadmap.md) | Feature maturity matrix (works / needs adjustment / stubbed / deprecated), the consolidated known-bug list, and suggested next steps. |

### Original author design notes

These three files predate this documentation set and are the author's own design
notes. They are kept for historical context; where they disagree with the code,
the documents above reflect the **current source of truth**.

- [`jackal_ir.md`](./jackal_ir.md) — early IR opcode list.
- [`jackal_compiler.md`](./jackal_compiler.md) — early compiler/instruction note.
- [`jackal_emits_if.md`](./jackal_emits_if.md) — intended `if` lowering (note: the
  actual `if` codegen does not yet match this; see [`ir.md`](./ir.md) and
  [`roadmap.md`](./roadmap.md)).

## Repository map

```
jackal/
├── jackal.c                 # CLI entry point: open file, yyparse()
├── jackal_lexer.l           # flex lexer source        ──► jackal_lexer.c (generated, in-tree)
├── jackal_parser.y          # bison grammar source     ──► jackal_parser.c/.h (generated, in-tree)
├── defs/keywords.gperf      # gperf keyword table (generated header is currently UNUSED)
├── include/
│   ├── jackal.h             # umbrella header (includes everything)
│   ├── jackal/*.h           # per-module public headers
│   └── check.h              # vendored "Check" unit-test framework
├── libjackal/               # the compiler library (static libjackal.a)
│   └── *.c                  # ast, compiler, ir, context, eval, optimizer,
│                            # hash, stack, string, symbol_table, class
├── samples/main.jkl         # example Jackal program
├── tests/                   # Check-based unit tests (compiler.c)
├── docs/                    # this documentation
├── configure.ac, Makefile.am, libjackal/Makefile.am   # autotools build
└── tools.mk                 # legacy helper targets (format/lint/deps) — partly stale
```

See [`architecture.md`](./architecture.md) to understand how these pieces fit
together.
