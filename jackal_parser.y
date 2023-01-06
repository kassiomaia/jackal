%{
#include <jackal.h>

extern int yylineno;
extern char *yytext;
extern int yylex(void);

jkl_program_t program;
jkl_node_t *cc;

#define DEBUG 0

%}

%union {
    char *id;
    char *string;
    int number;
    float fnumber;
    jkl_node_t node;
    jkl_node_t block;
}

%token <id>       ID
%token <number>   CINT
%token <fnumber>  CFLOAT
%token <string>   CSTRING

%token LET        "let"
%token ASSIGN     ":="
%token LBRACE     "{"
%token RBRACE     "}"
%token LOOP       "loop"
%token RAISE      "raise"
%token PUTS       "puts"

%type <node>  expr
%type <node>  ident
%type <node>  loop
%type <node>  statement
%type <block> statements
%type <node>  block
%type <node>  puts

%%

program: { jkl_program_init(&program); }
       | program statements {
          jkl_node_t *block = &$2;
          if (block->type == JKL_NODE_BLOCK) {
            printf("[jkl_parse] emit program block\n");
            jkl_emit_block(&program, block);
          } else {
            printf("[jkl_parse] the program is not a block\n");
            exit(1);
          }

          #if DEBUG
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
          | statements statement {
            jkl_node_t *block = &$$;
            block->type = JKL_NODE_BLOCK;

            jkl_node_append(block, &$2);
          }
          ;

statement: LET ident ASSIGN expr {
            jkl_node_t ident = $2;
            jkl_node_t expr = $4;

            if (ident.type != JKL_NODE_ID) {
              yyerror("[jkl_parser] Expected identifier");
              YYERROR;
            } else {
              jkl_node_t binop = JKL_AST_BINOP(&ident, JKL_OP_ASSIGN, &expr);
              $$ = binop;
            }
         }
         | loop
         | RAISE CSTRING {
            $$ = JKL_AST_RAISE($2);
         }
         | puts
         ;

expr: ID                              { $$ = JKL_AST_ID($1);      }
    | CINT                            { $$ = JKL_AST_INT($1);     }
    | CSTRING                         { $$ = JKL_AST_STRING($1);  }
    | CFLOAT                          { $$ = JKL_AST_FLOAT($1);   }
    ;

ident: ID {
        $$ = (jkl_node_t){.type = JKL_NODE_ID, .value = { .s = {$1} } };
      }
      ;

loop: LOOP block {
        jkl_node_t *block = &$2;
        jkl_node_t loop = JKL_AST_LOOP(block);

        $$ = loop;
      }
      ;

block: LBRACE statements RBRACE { $$ = $2; }
     ;

puts: PUTS CSTRING      {
        jkl_node_t str = JKL_AST_STRING($2);
        $$ = JKL_AST_PUTS(&str);
      }
    | PUTS ident        { $$ = JKL_AST_PUTS(&$2); }
    ;

%%

int yyerror(char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  fprintf(stderr, "near %s\n", yytext);
  fprintf(stderr, "%s\n", s);
  return 0;
}

int yywrap(void) {
  return 1;
}
