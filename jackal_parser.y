%{
#include <jackal.h>

extern int yylineno;
extern char *yytext;
extern int yylex(void);
int yyerror(char*);

jkl_program_t *program;

#define USE_EVAL
#undef USE_EVAL

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
%token IF     "if"
%token ELIF   "elif"
%token ELSE   "else"
%token FUNC   "func"
%token RETURN "return"
%token TRUE   "true"
%token FALSE  "false"
%token LPAREN "("
%token RPAREN ")"
%token LBRACK "["
%token RBRACK "]"
%token COMMA  ","
%token DOT    "."
%token PIPE   "|"

%type <node> expr
%type <node> arglist
%type <node> block_arg
%type <node> block_params
%type <node> param_seq
%type <node> ident
%type <node> call
%type <node> func
%type <node> term
%type <node> param
%type <node> func_params
%type <node> if_then

%type statement
%type statements

/* Lowest to highest precedence (bison: later declarations bind tighter). */
%left  OR
%left  AND
%left  EQL NEQ
%nonassoc GT GTE LT LTE
%left  PLUS MINUS
%left  MUL DIV MOD

%%

program: { 
          program = jkl_program_new();
          program->ast_prog_root = jkl_node_new(JKL_NODE_BLOCK);
          jkl_push_context(program, program->ast_prog_root);
        }
       | program statements {
          jkl_pop_context(program);
          jkl_ensure_empty_contexts();
        }
       ;

statements:
          | statements statement
          | statement
          ;

statement: LET ident ASSIGN expr {
            jkl_node_t* ident = $2;
            jkl_node_t* expr = $4;

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            if (ident->type != JKL_NODE_ID) {
              yyerror("expected an identifier");
              YYERROR;
            } 

            jkl_node_t* let = jkl_node_new(JKL_NODE_LET);
            let->id = ident;
            let->expr = expr;

            jkl_log("jkl_parser", "emit statement: %p", let);
            jkl_node_append(jkl_get_context(program), let);
         }
         | loop
         | RAISE CSTRING {
            jkl_node_t* raise = jkl_node_new(JKL_NODE_RAISE);
            raise->value.s = $2;

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "raise: %s", $2);

            jkl_node_append(jkl_get_context(program), raise);
         }
         | call {
            jkl_note("jkl_parser", "emit ast call");
            jkl_node_t* call = $1;

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "call: %p", call);

            jkl_node_append(jkl_get_context(program), call);
          }
         | if_stm
         | RETURN expr {
            jkl_node_t* ret = jkl_node_new(JKL_NODE_RETURN);
            ret->expr = $2;

            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_log("jkl_parser", "return: %p", ret);

            jkl_node_append(jkl_get_context(program), ret);
         }
         | func {
            if (jkl_get_context(program) == NULL)
              jkl_error("jkl_parser", "no current context");

            jkl_node_append(jkl_get_context(program), $1);
         }
         ;

expr: expr PLUS  expr { $$ = jkl_node_binop($1, JKL_OP_PLUS,  $3); }
    | expr MINUS expr { $$ = jkl_node_binop($1, JKL_OP_MINUS, $3); }
    | expr MUL   expr { $$ = jkl_node_binop($1, JKL_OP_MUL,   $3); }
    | expr DIV   expr { $$ = jkl_node_binop($1, JKL_OP_DIV,   $3); }
    | expr MOD   expr { $$ = jkl_node_binop($1, JKL_OP_MOD,   $3); }
    | expr EQL   expr { $$ = jkl_node_binop($1, JKL_OP_EQL,   $3); }
    | expr NEQ   expr { $$ = jkl_node_binop($1, JKL_OP_NEQ,   $3); }
    | expr GT    expr { $$ = jkl_node_binop($1, JKL_OP_GT,    $3); }
    | expr GTE   expr { $$ = jkl_node_binop($1, JKL_OP_GTE,   $3); }
    | expr LT    expr { $$ = jkl_node_binop($1, JKL_OP_LT,    $3); }
    | expr LTE   expr { $$ = jkl_node_binop($1, JKL_OP_LTE,   $3); }
    | expr AND   expr { $$ = jkl_node_binop($1, JKL_OP_AND,   $3); }
    | expr OR    expr { $$ = jkl_node_binop($1, JKL_OP_OR,    $3); }
    | LPAREN expr RPAREN { $$ = $2; }
    | term               { $$ = $1; }
    ;

term: ident
    | CINT {
      jkl_node_t* cint = jkl_node_new(JKL_NODE_INT);
      cint->value.i = $1;

      jkl_log("jkl_parser", "emit ast cint: %d", $1);

      $$ = cint;
    }
    | CSTRING {
      jkl_node_t* cstring = jkl_node_new(JKL_NODE_STRING);
      cstring->value.s = $1;

      jkl_log("jkl_parser", "emit ast cstring: %s", $1);

      $$ = cstring;
    }
    | CFLOAT {
      jkl_node_t* cfloat = jkl_node_new(JKL_NODE_FLOAT);
      cfloat->value.f = $1;

      jkl_log("jkl_parser", "emit ast cfloat: %f", $1);

      $$ = cfloat;
    }
    | TRUE {
      jkl_node_t* b = jkl_node_new(JKL_NODE_BOOL);
      b->value.i = 1;
      $$ = b;
    }
    | FALSE {
      jkl_node_t* b = jkl_node_new(JKL_NODE_BOOL);
      b->value.i = 0;
      $$ = b;
    }
    | term DOT ID {
      jkl_node_t* m = jkl_node_new(JKL_NODE_METHOD_CALL);
      m->node = $1;
      m->id = jkl_node_new(JKL_NODE_ID);
      m->id->value.s = $3;
      m->params = NULL;
      $$ = m;
    }
    | term DOT ID LPAREN RPAREN {
      jkl_node_t* m = jkl_node_new(JKL_NODE_METHOD_CALL);
      m->node = $1;
      m->id = jkl_node_new(JKL_NODE_ID);
      m->id->value.s = $3;
      m->params = jkl_node_new(JKL_NODE_PARAMS);
      $$ = m;
    }
    | term DOT ID LPAREN arglist RPAREN {
      jkl_node_t* m = jkl_node_new(JKL_NODE_METHOD_CALL);
      m->node = $1;
      m->id = jkl_node_new(JKL_NODE_ID);
      m->id->value.s = $3;
      m->params = $5;
      $$ = m;
    }
    | term DOT ID block_arg {
      jkl_node_t* m = jkl_node_new(JKL_NODE_METHOD_CALL);
      m->node = $1;
      m->id = jkl_node_new(JKL_NODE_ID);
      m->id->value.s = $3;
      m->params = NULL;
      m->block_arg = $4;
      $$ = m;
    }
    | term DOT ID LPAREN RPAREN block_arg {
      jkl_node_t* m = jkl_node_new(JKL_NODE_METHOD_CALL);
      m->node = $1;
      m->id = jkl_node_new(JKL_NODE_ID);
      m->id->value.s = $3;
      m->params = jkl_node_new(JKL_NODE_PARAMS);
      m->block_arg = $6;
      $$ = m;
    }
    | term DOT ID LPAREN arglist RPAREN block_arg {
      jkl_node_t* m = jkl_node_new(JKL_NODE_METHOD_CALL);
      m->node = $1;
      m->id = jkl_node_new(JKL_NODE_ID);
      m->id->value.s = $3;
      m->params = $5;
      m->block_arg = $7;
      $$ = m;
    }
    | LBRACK RBRACK {
      jkl_node_t* a = jkl_node_new(JKL_NODE_ARRAY_LIT);
      $$ = a;
    }
    | LBRACK arglist RBRACK {
      jkl_node_t* a = jkl_node_new(JKL_NODE_ARRAY_LIT);
      a->compound = $2->compound;     /* steal the PARAMS' child array */
      $2->compound.nodes = NULL;
      $2->compound.n_nodes = 0;
      jkl_node_free($2);
      $$ = a;
    }
    | term LBRACK expr RBRACK {
      /* sugar: arr[i]  ==  arr.at(i)  */
      jkl_node_t* m = jkl_node_new(JKL_NODE_METHOD_CALL);
      m->node = $1;
      m->id = jkl_node_new(JKL_NODE_ID);
      m->id->value.s = strdup("at");
      m->params = jkl_node_new(JKL_NODE_PARAMS);
      jkl_node_append(m->params, $3);
      $$ = m;
    }
    ;

block_arg: LBRACE block_params expr RBRACE {
       jkl_node_t* b = jkl_node_new(JKL_NODE_BLOCK_LIT);
       b->params = $2;
       b->expr = $3;
       $$ = b;
     }
     ;

block_params: /* empty */ {
       $$ = jkl_node_new(JKL_NODE_PARAMS);
     }
     | PIPE PIPE {
       $$ = jkl_node_new(JKL_NODE_PARAMS);
     }
     | PIPE param_seq PIPE {
       $$ = $2;
     }
     ;

param_seq: ident {
       jkl_node_t* params = jkl_node_new(JKL_NODE_PARAMS);
       jkl_node_t* p = jkl_node_new(JKL_NODE_PARAM);
       p->id = $1;
       jkl_node_append(params, p);
       $$ = params;
     }
     | param_seq COMMA ident {
       jkl_node_t* p = jkl_node_new(JKL_NODE_PARAM);
       p->id = $3;
       jkl_node_append($1, p);
       $$ = $1;
     }
     ;

arglist: expr {
       jkl_node_t* params = jkl_node_new(JKL_NODE_PARAMS);
       jkl_node_append(params, $1);
       $$ = params;
     }
     | arglist COMMA expr {
       jkl_node_append($1, $3);
       $$ = $1;
     }
     ;

ident: ID {
        jkl_node_t* ident = jkl_node_new(JKL_NODE_ID);
        ident->value.s = $1;

        jkl_log("jkl_parser", "emit ast ident: %s", $1);

        $$ = ident;
      }
      ;

loop: LOOP LBRACE {
        jkl_note("jkl_parser", "begin emit ast loop");
        jkl_note("jkl_parser", "begin emit ast block");
        jkl_node_t* block = jkl_node_new(JKL_NODE_BLOCK);
        jkl_push_context(program, block);
      }
      block_stmts
      RBRACE {
        jkl_note("jkl_parser", "end emit ast block");
        jkl_node_t* block = jkl_pop_context(program);

        jkl_node_t *loop = jkl_node_new(JKL_NODE_LOOP);
        loop->block = block;

        jkl_node_t *context = jkl_get_context(program);
        if (context == NULL)
          jkl_error("jkl_parser", "no current context");

        jkl_node_append(context, loop);
        jkl_log("jkl_parser", "emit ast loop");
      }
      ;

if_stm: if_then else_opt
      ;

if_then: IF expr {
          jkl_note("jkl_parser", "begin emit ast if");
          jkl_node_t* if_node = jkl_node_new(JKL_NODE_IF);
          if_node->expr = $2;
          if_node->block = jkl_node_new(JKL_NODE_BLOCK);
          jkl_node_append(jkl_get_context(program), if_node);

          jkl_push_context(program, if_node->block);
          jkl_note("jkl_parser", "begin emit ast if block");
          $<node>$ = if_node;
        } LBRACE block_stmts RBRACE {
          jkl_pop_context(program);
          jkl_note("jkl_parser", "end emit ast if then");
          $$ = $<node>3;
        }
      ;

else_opt:
        /* no else */
      | ELSE {
          jkl_node_t* if_node = $<node>0;
          if_node->block_else = jkl_node_new(JKL_NODE_BLOCK);
          jkl_push_context(program, if_node->block_else);
          jkl_note("jkl_parser", "begin emit ast else block");
        } LBRACE block_stmts RBRACE {
          jkl_pop_context(program);
          jkl_note("jkl_parser", "end emit ast else");
        }
      | ELIF expr {
          /* desugar `elif` into `else { if ... }` */
          jkl_node_t* outer = $<node>0;
          outer->block_else = jkl_node_new(JKL_NODE_BLOCK);
          jkl_push_context(program, outer->block_else);

          jkl_node_t* inner = jkl_node_new(JKL_NODE_IF);
          inner->expr = $2;
          inner->block = jkl_node_new(JKL_NODE_BLOCK);
          jkl_node_append(jkl_get_context(program), inner);
          jkl_push_context(program, inner->block);
          jkl_note("jkl_parser", "begin emit ast elif");
          $<node>$ = inner;
        } LBRACE block_stmts RBRACE {
          jkl_pop_context(program);
          $<node>$ = $<node>3;
        } else_opt {
          jkl_pop_context(program);
          jkl_note("jkl_parser", "end emit ast elif");
        }
      ;

block_stmts:
           | block_stmts statement
           | statement
           ;

call: ID CSTRING {
      jkl_node_t* callee = jkl_node_new(JKL_NODE_ID);
      callee->value.s = $1;

      jkl_node_t* cstring = jkl_node_new(JKL_NODE_STRING);
      cstring->value.s = $2;

      jkl_node_t* call = jkl_node_new(JKL_NODE_CALL);
      call->id = callee;
      call->node = cstring;

      jkl_log("jkl_parser", "emit ast call: %s", $1);

      $$ = call;
    }
    | ID ident {
      jkl_node_t* callee = jkl_node_new(JKL_NODE_ID);
      callee->value.s = $1;

      jkl_node_t* call = jkl_node_new(JKL_NODE_CALL);
      call->id = callee;
      call->node = $2;

      jkl_log("jkl_parser", "emit ast call: %s", $1);

      $$ = call;
    }
    ;

func: FUNC ident func_params LBRACE {
        jkl_node_t* block = jkl_node_new(JKL_NODE_BLOCK);
        jkl_push_context(program, block);
      } block_stmts RBRACE {
        jkl_node_t* block = jkl_pop_context(program);
        jkl_node_t* func = jkl_node_new(JKL_NODE_FUNC);

        func->id = $2;
        func->params = $3;
        func->block = block;

        jkl_log("jkl_parser", "emit ast func: %s", $2);

        $$ = func;
      }
    ;

func_params: LPAREN {
            jkl_node_t* params = jkl_node_new(JKL_NODE_PARAMS);
            jkl_push_context(program, params);
           } params RPAREN {
            $$ = jkl_pop_context(program);
           }
           ;

params:
      | params COMMA param {
        jkl_node_t* params = jkl_pop_context(program);
        jkl_node_append(params, $3);
        jkl_push_context(program, params);
      }
      | param {
        jkl_node_t* params = jkl_pop_context(program);
        jkl_node_append(params, $1);
        jkl_push_context(program, params);
      }
      ;

param: ident {
      jkl_node_t* param = jkl_node_new(JKL_NODE_PARAM);
      param->id = $1;

      jkl_log("jkl_parser", "emit ast param: %s", $1);

      $$ = param;
    }
    ;

%%

int yyerror(char *s) {
  jkl_error("jkl_parser", "line %d: %s near '%s'", yylineno, s, yytext);
}

int yywrap(void) {
  return 1;
}
