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
  char *id;
  char *string;
  int number;
  float fnumber;
  jkl_node_t *node;
}

%token <id>      ID
%token <number>  CINT
%token <fnumber> CFLOAT
%token <string>  CSTRING

%token LET    "let"
%token ASSIGN ":="
%token LBRACE "{"
%token RBRACE "}"
%token LOOP   "loop"
%token RAISE  "raise"
%token PUTS   "puts"

%type <node> expr
%type <node> ident
%type <node> puts

%type statement
%type statements

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

            jkl_node_t* binop = jkl_node_new(JKL_NODE_BINOP);
            binop->binop.op = JKL_OP_ASSIGN;
            binop->binop.left = ident;
            binop->binop.right = expr;

            jkl_log("jkl_parser", "emit statement: %p", binop);
            jkl_print_ast_type(binop);

            jkl_node_append(jkl_get_context(&program), binop);
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
         ;

expr: ID {
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
