%{
#include <jackal.h>
#include <jackal_parser.h>
%}

int [0-9]+
string ["].+["]
float [+|-]?{int}?[.]{int}

%x STR
%x CMT

%%

[ \t]+

"let"                                   { return LET;    }
"loop"                                  { return LOOP;   }
"raise"                                 { return RAISE;  }
"if"                                    { return IF;     }
"elif"                                  { return ELIF;   }
"else"                                  { return ELSE;   }
"func"                                  { return FUNC;   }
"return"                                { return RETURN; }
":="                                    { return ASSIGN; }
"{"                                     { return LBRACE; }
"}"                                     { return RBRACE; }
"=="                                    { return EQL;    }
"!="                                    { return NEQ;    }
">"                                     { return GT;     }
"<"                                     { return LT;     }
">="                                    { return GTE;    }
"<="                                    { return LTE;    }
"+"                                     { return PLUS;   }
"-"                                     { return MINUS;  }
"*"                                     { return MUL;    }
"/"                                     { return DIV;    }
"%"                                     { return MOD;    }
"&&"                                    { return AND;    }
"||"                                    { return OR;     }
"!"                                     { return NOT;    }
"("                                     { return LPAREN; }
")"                                     { return RPAREN; }
","                                     { return COMMA;  }

[a-zA-Z_][a-zA-Z0-9_]*                  {
  yylval.id = malloc(strlen(yytext) + 1);
  strncpy(yylval.id, yytext, strlen(yytext) + 1);
  return ID;
}

{int} {
  yylval.number = atoi(yytext);
  return CINT;
}

{float} {
  yylval.fnumber = atof(yytext);
  return CFLOAT;
}

"/*"      { BEGIN(CMT); }
<CMT>"*/" { BEGIN(INITIAL); }

["] {
  BEGIN(STR);
  yytext[0] = '\0';
}

<STR>[^"]+ {
  yylval.string = malloc(strlen(yytext) + 1);
  strncpy(yylval.string, yytext, strlen(yytext) + 1);
}

<STR>["] {
  BEGIN(INITIAL);
  for (jkl_char_t *p = yylval.string; *p; p++) {
    if (*p == '\\') {
      switch (*(p + 1)) {
        case 'n':
          *p = '\n';
          break;
        case 't':
          *p = '\t';
          break;
        case 'r':
          *p = '\r';
          break;
        case 'v':
          *p = '\v';
          break;
        case 'b':
          *p = '\b';
          break;
        case 'f':
          *p = '\f';
          break;
        case 'a':
          *p = '\a';
          break;
        case '\\':
          *p = '\\';
          break;
        case '\'':
          *p = '\'';
          break;
        case '"':
          *p = '"';
          break;
        default:
          jkl_error("jkl_lexer", "unknown escape sequence: \\%c", *(p + 1));
      }
      for (jkl_char_t *q = p + 1; *q; q++) {
        *q = *(q + 1);
      }
    }
  }

  return CSTRING;
}

. {
  jkl_error("jkl_lexer", "unknow char code %s\n", yytext);
}

%%
