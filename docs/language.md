# The Jackal Language

Jackal is a small imperative language. Source files use the `.jkl` extension.
This document describes the language **as the lexer and parser actually accept
it today** (`jackal_lexer.l`, `jackal_parser.y`), and flags the gaps between what
is tokenized and what the grammar can parse.

A complete example lives in [`../samples/main.jkl`](../samples/main.jkl):

```jackal
let doc := 1
let name := "John"
let projetos := 10
let first_name := "Kássio Maia"
let second_name := "de Queiroz"
let full_name := "Kassio Maia" + 10
let n := 2.35

if doc == 1 {
  puts "OK"
}

puts "OK"

loop {
  let projetos1 := 10
  if a > 10 {
    puts "OK"
  }
}

func main(argc, argv) {

}
```

## Lexical structure

Defined in `jackal_lexer.l`.

### Keywords (`jackal_lexer.l:17-24`)

`let`, `loop`, `raise`, `if`, `elif`, `else`, `func`, `return`

> `elif` and `else` are tokenized but **no grammar rule consumes them** — see
> [Unsupported / incomplete](#unsupported--incomplete-syntax).

### Operators & punctuation (`jackal_lexer.l:25-44`)

| Category | Tokens |
|----------|--------|
| Assignment | `:=` |
| Comparison | `==` `!=` `>` `<` `>=` `<=` |
| Arithmetic | `+` `-` `*` `/` `%` |
| Logical | `&&` `\|\|` `!` |
| Grouping/delimiters | `(` `)` `{` `}` `,` |

### Literals

- **Integer** — `[0-9]+`, parsed with `atoi` into `CINT` (`jackal_lexer.l:52`).
- **Float** — `[+|-]?{int}?[.]{int}`, parsed with `atof` into `CFLOAT`
  (`jackal_lexer.l:57`). Requires a decimal point; e.g. `2.35`, `.5`.
- **String** — double-quoted, handled via the `STR` start-condition
  (`jackal_lexer.l:65-120`). Supported escapes: `\n \t \r \v \b \f \a \\ \' \"`.
  An unknown escape is a fatal lexer error.
- **Boolean** — the keywords `true` / `false` (`JKL_NODE_BOOL`), lowered to
  `PUSHB 1/0`.

### Identifiers (`jackal_lexer.l:46`)

`[a-zA-Z_][a-zA-Z0-9_]*`. Note the regex is ASCII-only, but string *contents*
may be UTF-8 (e.g. `"Kássio Maia"`).

### Comments (`jackal_lexer.l:62-63`)

Block comments only: `/* ... */`. There are **no** line comments, no nesting, and
no end-of-file-inside-comment handling.

### Whitespace

Spaces and tabs are skipped (`jackal_lexer.l:15`). Newlines are not explicitly
handled (flex tracks `yylineno` for error messages).

## Grammar

From `jackal_parser.y`. Shown as EBNF-style pseudo-grammar (terminals in the
lexer column above). `ε` = empty.

```
program     ::= ε
              | program statements

statements  ::= ε
              | statements statement
              | statement

statement   ::= "let" ident ":=" expr        # variable declaration
              | loop
              | "raise" CSTRING               # raise with a string message
              | call
              | if_stm
              | "return" expr
              | func

expr        ::= term op term
              | expr op expr
              | "(" expr ")"
              | term

op          ::= "==" | "!=" | ">" | "<" | ">=" | "<="
              | "+"  | "-"  | "*" | "/" | "%"
              | "&&" | "||"

term        ::= ident | CINT | CSTRING | CFLOAT

ident       ::= ID

loop        ::= "loop" "{" block_stmts "}"

if_stm      ::= if_then else_opt
if_then     ::= "if" expr "{" block_stmts "}"
else_opt    ::= ε
              | "else" "{" block_stmts "}"
              | "elif" expr "{" block_stmts "}" else_opt

block_stmts ::= ε
              | block_stmts statement
              | statement

call        ::= ID CSTRING                    # e.g.  puts "OK"
              | ID ident                       # e.g.  print x

func        ::= "func" ident func_params "{" block_stmts "}"

func_params ::= "(" params ")"

params      ::= ε
              | params "," param
              | param

param       ::= ident
```

## Constructs

### Variable declaration — `let`

```jackal
let name := expr
```

There is no separate assignment statement; `let` is the only way to bind a name,
and it can re-declare (re-declaring reuses the name's storage). The right-hand
side is any `expr`. Produces `JKL_NODE_LET { id, expr }`.

Names resolve to **storage slots** through a symbol table: a `let` allocates (or
reuses) a slot, and a later use of the name loads that slot. **Using a name that
was never declared with `let` is a compile error** (`undeclared identifier '…'`),
so declare before use.

### Expressions and operators

Binary expressions: `expr op expr`, a parenthesized `( expr )`, or a bare
`term`. There are **no unary expressions** (see below). Produces
`JKL_NODE_BINOP { left, op, right }`.

**Operator precedence is enforced.** The operators are inlined into the `expr`
rule as terminals, each carrying the precedence/associativity of its `%left` /
`%nonassoc` declaration (`jackal_parser.y`). From lowest to highest binding:

| Precedence (low → high) | Operators | Associativity |
|-------------------------|-----------|---------------|
| 1 | `\|\|` | left |
| 2 | `&&` | left |
| 3 | `==` `!=` | left |
| 4 | `<` `<=` `>` `>=` | non-associative |
| 5 | `+` `-` | left |
| 6 | `*` `/` `%` | left |

So `1 + 2 * 3` groups as `1 + (2 * 3)`, `1 - 2 - 3` as `(1 - 2) - 3`, and
`1 + 2 == 3` as `(1 + 2) == 3`. Relational operators are non-associative, so
`a < b < c` is a syntax error — parenthesize it. Use `( … )` to override the
default grouping anywhere.

### Conditionals — `if` / `elif` / `else`

```jackal
if cond {
  ...
} elif other {
  ...
} else {
  ...
}
```

The condition is **not** parenthesized in the grammar (`if expr { ... }`), even
though `../samples/main.jkl` and the design note
[`jackal_emits_if.md`](./jackal_emits_if.md) show parentheses — parentheses work
only because `( expr )` is itself a valid `expr`. Produces
`JKL_NODE_IF { expr, block, block_else }`.

`else` and any number of `elif` branches are optional. `elif` is **desugared**
by the parser into `else { if ... }`, so the AST only ever has a single optional
`block_else` (which may itself contain a nested `JKL_NODE_IF`). Blocks are
brace-delimited, so there is no dangling-else ambiguity. See
[`ir.md`](./ir.md#how-constructs-are-lowered) for the lowering.

### Loops — `loop`

```jackal
loop {
  ...
}
```

An unconditional (infinite) loop; the only loop construct. There is no `for`,
no `while`, and no `break`/`continue`. Produces `JKL_NODE_LOOP { block }`.

### Calls — `call`

Two forms, both `<ID> <argument>` with no parentheses:

```jackal
puts "OK"      # ID CSTRING  → call with a string argument
print x        # ID ident    → call with an identifier argument
```

`puts` is **not** a keyword — it is just an identifier used in the `ID CSTRING`
call form. There is no general call syntax (no `f(a, b)`, exactly one argument,
no parentheses). Produces `JKL_NODE_CALL { id, node }` where `id` is the callee
name and `node` is the single argument. A call is a **statement**, not an
expression, so a return value cannot be consumed yet (it is left on the stack).
The compiler resolves the callee to an **internal** `CALL` (into a user `func`)
or, if the name has no definition, an **external/builtin** call (e.g. `puts`).
See the calling convention in [`ir.md`](./ir.md).

### Method calls — `recv.method(args)`

Unlike `call`, method calls are **expressions** with Ruby-style `.` syntax, used to
invoke methods on primitive values:

```jackal
let s := "hello"
let n := s.length            # no-arg
let u := s.upcase
let c := "ab".concat("cd")   # with args
```

Produces `JKL_NODE_METHOD_CALL { node=receiver, id=method-name, params=args }` and
lowers to `SEND` (dynamic dispatch on the receiver's type). Method calls are
postfix on `term`, so they bind tighter than every operator (`s.length + 1` is
`(s.length) + 1`) and chain (`a.b.c`). The available primitive methods and how to
add more are documented in [`types.md`](./types.md).

### Arrays and iterators

Array literals use `[...]` with comma-separated expressions; `arr[i]` is sugar
for `arr.at(i)`. Iterator methods take a trailing block:

```jkl
let a := [1, 2, 3]
let n := a.length
let x := a[1]
let r := a.map    { |x| x.succ }
let s := a.reduce(0) { |acc, x| acc + x }
let _ := a.each   { |x| puts x.to_s }
```

Blocks are `{ |params...| body-expr }` — exactly one expression body, no
closures yet (just the param env), max 8 params. The method-call rule on `term`
accepts a trailing block whether the parens are present (`a.reduce(0) { … }`)
or absent (`a.each { … }`). Empty parens (`{ || expr }`) and empty params
(`{ puts "hi" }`) are both accepted.

Full memory model, the implemented method set, and the rationale for why
iterators don't serialize to bytecode are in [`arrays.md`](./arrays.md).

### Functions — `func`

```jackal
func main(argc, argv) {
  ...
}
```

Produces `JKL_NODE_FUNC { id, params, block }`, with `params` a
`JKL_NODE_PARAMS` of `JKL_NODE_PARAM` children. Parameters are bare identifiers
separated by commas.

Top-level functions are compiled: the body is **hoisted** after the program's
`HALT` and entered only via `CALL`, with a prologue that binds the parameter and
a trailing `RET` (see [`ir.md`](./ir.md)). Limitations today: calls pass a single
argument, so a function with more than one parameter binds only the first (the
rest are declared but unbound, with a compile-time warning); variables use a flat
slot space, so **recursion and per-function scopes are not supported yet**.

### Return — `return`

```jackal
return expr
```

`return` requires an expression (there is no bare `return`). Produces
`JKL_NODE_RETURN { expr }`.

### Raise — `raise`

```jackal
raise "some message"
```

Takes a string literal only. Produces `JKL_NODE_RAISE { value.s }`. There is no
exception type or catch construct; the compiler does not yet lower `raise` (it
would error). See [`roadmap.md`](./roadmap.md).

## Unsupported / incomplete syntax

These are recognized by the lexer or named in the source but **cannot be used**:

- **`!` (logical NOT)** — tokenized and present in `jkl_op_t` as `JKL_OP_NOT`,
  but there is no unary-expression rule, so `!x` does not parse. (`op` only
  covers binary operators.)
- **`for`** — appears in `defs/keywords.gperf` only; not a token, not in the
  grammar. A bare `for` lexes as an identifier.
- **Assignment as a statement** (`x := 5` without `let`) — `JKL_NODE_ASSIGNMENT`
  exists in the enum but is never parsed or produced.
- **General function calls** `f(a, b)` — not supported; calls are `ID <one arg>`
  with no parentheses.

## Keyword table note (`defs/keywords.gperf`)

`defs/keywords.gperf` defines a gperf perfect-hash keyword table (with a
`has_block` flag per keyword), and a generated `keyword_lookup()` lives in
`include/jackal/jackal_keywords.h`. **None of it is wired into the lexer** — the
lexer matches keywords directly with string literals — so the gperf table and the
generated header are currently dead code.
