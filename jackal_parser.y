%{
#include <jackal.h>

extern int yylineno;
extern char *yytext;
extern int yylex(void);
int yyerror(char*);

#define MAX_NESTED_BLOCKS 1024

jkl_program_t program;
jkl_node_t *cc;

struct {
  jkl_node_t* frames[MAX_NESTED_BLOCKS];
  int pos;
} context;

jkl_node_t* jkl_get_context(jkl_program_t *program) {
  if (context.pos == 0) {
    return NULL;
  }

  jkl_log("jkl_parser", "context.pos = %d", context.pos);
  return context.frames[context.pos - 1];
}

void jkl_push_context(jkl_program_t *program, jkl_node_t *node) {
  if (jkl_get_context(program) != NULL) {
    jkl_node_t* parent = jkl_get_context(program);
    node->parent = parent;
  }

  jkl_log("jkl_parser", "pushing context");
  context.frames[context.pos] = node;
  context.pos++;
}

jkl_node_t* jkl_pop_context(jkl_program_t *program) {
  context.pos--;
  jkl_log("jkl_parser", "popping context");
  return context.frames[context.pos];
}

void jkl_ensure_empty_contexts() {
  if (context.pos != 0)
    jkl_error("jkl_error", "context stack is not empty");
}

%}

%union {
  char        *id;
  char        *string;
  int         number;
  float       fnumber;
  jkl_node_t  *node;
  jkl_op_t    op;
}

%token <id>      ID
%token <number>  CINT
%token <fnumber> CFLOAT
%token <string>  CSTRING

%token LET    "let"
%token ASSIGN ":="
%token LBRACE "{"
%token RBRACE "}"
%token EQL    "=="
%token NEQ    "!="
%token GT     ">"
%token LT     "<"
%token GTE    ">="
%token LTE    "<="
%token PLUS   "+"
%token MINUS  "-"
%token MUL    "*"
%token DIV    "/"
%token MOD    "%"
%token AND    "&&"
%token OR     "||"
%token NOT    "!"
%token LOOP   "loop"
%token RAISE  "raise"
%token PUTS   "puts"
%token IF     "if"
%token ELIF   "elif"
%token ELSE   "else"
%token LPAREN "("
%token RPAREN ")"

%type <node> expr
%type <node> ident
%type <node> puts
%type <node> term
%type <op>   op

%type statement
%type statements

%left PLUS MINUS
%left MUL DIV MOD
%left AND OR

%%

program: { 
          jkl_program_init(&program);
          jkl_node_t* block = jkl_node_new(JKL_NODE_BLOCK);
          jkl_push_context(&program, block);
        }
       | program statements {
          jkl_node_t* block = jkl_pop_context(&program);
          jkl_ensure_empty_contexts();

          jkl_word_t n = jkl_compile(&program, block);
          if (n != 0) {
            jkl_error("jkl_error", "compilation failed");
          }
          jkl_node_free(block);

#ifdef DUMP
          jkl_dump(&program);
#else
          jkl_vm_t vm;
          jkl_vm_init(&vm);
          jkl_vm_load(&vm, &program);
          jkl_vm_run(&vm);
#endif
        }
       ;

statements:
          | statements statement
          | statement
          ;

statement: LET ident ASSIGN expr {
            jkl_node_t* ident = $2;
            jkl_node_t* expr = $4;

            if (jkl_get_context(&program) == NULL)
              jkl_error("jkl_parser", "no current context");

            if (ident->type != JKL_NODE_ID) {
              yyerror("expected an identifier");
              YYERROR;
            } 

            jkl_node_t* let = jkl_node_new(JKL_NODE_LET);
            let->id = ident;
            let->assign = expr;

            jkl_log("jkl_parser", "emit statement: %p", let);
            jkl_print_ast_type(let);

            jkl_node_append(jkl_get_context(&program), let);
         }
         | loop
         | RAISE CSTRING {
            jkl_node_t* raise = jkl_node_new(JKL_NODE_RAISE);
            raise->value.s = $2;

            if (jkl_get_context(&program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "raise: %s", $2);
            jkl_print_ast_type(raise);

            jkl_node_append(jkl_get_context(&program), raise);
         }
         | puts {
            jkl_note("jkl_parser", "emit ast puts");
            jkl_node_t* puts = $1;

            if (jkl_get_context(&program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "puts: %p", puts);
            jkl_print_ast_type(puts);

            jkl_node_append(jkl_get_context(&program), puts);
          }
         | if_stm
         ;

expr: term op term {
      jkl_node_t* binop = jkl_node_new(JKL_NODE_BINOP);
      binop->binop.op = $2;
      binop->binop.left = $1;
      binop->binop.right = $3;
      $$ = binop;
    }
    | expr op expr {
      jkl_node_t* binop = jkl_node_new(JKL_NODE_BINOP);
      binop->binop.op = $2;
      binop->binop.left = $1;
      binop->binop.right = $3;
      $$ = binop;
    }
    | LPAREN expr RPAREN {
      $$ = $2;
    }
    | term {
      $$ = $1;
    }
    ;

op: EQL   { $$ = JKL_OP_EQL; }
  | NEQ   { $$ = JKL_OP_NEQ; }
  | GT    { $$ = JKL_OP_GT; }
  | LT    { $$ = JKL_OP_LT; }
  | GTE   { $$ = JKL_OP_GTE; }
  | LTE   { $$ = JKL_OP_LTE; }
  | PLUS  { $$ = JKL_OP_PLUS; }
  | MINUS { $$ = JKL_OP_MINUS; }
  | MUL   { $$ = JKL_OP_MUL; }
  | DIV   { $$ = JKL_OP_DIV; }
  | MOD   { $$ = JKL_OP_MOD; }
  | AND   { $$ = JKL_OP_AND; }
  | OR    { $$ = JKL_OP_OR; }
  ;

term: ID {
      jkl_node_t* ident = jkl_node_new(JKL_NODE_ID);
      ident->value.s = $1;

      jkl_log("jkl_parser", "emit ast ident: %s", $1);
      jkl_print_ast_type(ident);

      $$ = ident;
    }
    | CINT {
      jkl_node_t* cint = jkl_node_new(JKL_NODE_INT);
      cint->value.i = $1;

      jkl_log("jkl_parser", "emit ast cint: %d", $1);
      jkl_print_ast_type(cint);

      $$ = cint;
    }
    | CSTRING {
      jkl_node_t* cstring = jkl_node_new(JKL_NODE_STRING);
      cstring->value.s = $1;

      jkl_log("jkl_parser", "emit ast cstring: %s", $1);
      jkl_print_ast_type(cstring);

      $$ = cstring;
    }
    | CFLOAT {
      jkl_node_t* cfloat = jkl_node_new(JKL_NODE_FLOAT);
      cfloat->value.f = $1;

      jkl_log("jkl_parser", "emit ast cfloat: %f", $1);
      jkl_print_ast_type(cfloat);

      $$ = cfloat;
    }
    ;

ident: ID {
        jkl_node_t* ident = jkl_node_new(JKL_NODE_ID);
        ident->value.s = $1;

        jkl_log("jkl_parser", "emit ast ident: %s", $1);
        jkl_print_ast_type(ident);

        $$ = ident;
      }
      ;

loop: LOOP LBRACE {
        jkl_note("jkl_parser", "begin emit ast loop");
        jkl_note("jkl_parser", "begin emit ast block");
        jkl_node_t* block = jkl_node_new(JKL_NODE_BLOCK);
        jkl_push_context(&program, block);
      }
      block_stmts
      RBRACE {
        jkl_note("jkl_parser", "end emit ast block");
        jkl_node_t* block = jkl_pop_context(&program);

        jkl_node_t *loop = jkl_node_new(JKL_NODE_LOOP);
        loop->block = block;

        jkl_node_t *context = jkl_get_context(&program);
        if (context == NULL)
          jkl_error("jkl_parser", "no current context");

        jkl_node_append(context, loop);
        jkl_log("jkl_parser", "emit ast loop");
      }
      ;

if_stm: IF expr {
        jkl_note("jkl_parser", "begin emit ast if");
        jkl_node_t* if_stm = jkl_node_new(JKL_NODE_IF);
        if_stm->node = $2;
        if_stm->block = jkl_node_new(JKL_NODE_BLOCK);
        jkl_node_append(jkl_get_context(&program), if_stm);

        jkl_push_context(&program, if_stm->block);
        jkl_note("jkl_parser", "begin emit ast if block");
      } LBRACE block_stmts RBRACE
      {
        jkl_pop_context(&program);
        jkl_note("jkl_parser", "end emit ast if");
      }
      ;

block_stmts:
           | block_stmts statement
           | statement
           ;

puts: PUTS CSTRING {
      jkl_node_t* cstring = jkl_node_new(JKL_NODE_STRING);
      cstring->value.s = $2;

      jkl_node_t* puts = jkl_node_new(JKL_NODE_PUTS);
      puts->node = cstring;

      jkl_log("jkl_parser", "emit ast puts: %s", $2);
      jkl_print_ast_type(puts);

      $$ = puts;
    }
    | PUTS ident {
      jkl_node_t* ident = $2;
      jkl_node_t* puts = jkl_node_new(JKL_NODE_PUTS);
      puts->node = ident;

      jkl_log("jkl_parser", "emit ast puts: %s", $2);
      jkl_print_ast_type(puts);

      $$ = puts;
    }
    ;

%%

int yyerror(char *s) {
  jkl_error("jkl_parser", "line %d: %s near '%s'", yylineno, s, yytext);
}

int yywrap(void) {
  return 1;
}
