%{
#include <jackal.h>
#include "jackal_parser.tab.h"
%}

int [0-9]+
string ["].+["]
float [+|-]?{int}?[.]{int}

%x STR

%%

[ \t]+

"let"                                   { return LET;     }
"loop"                                  { return LOOP;    }
"raise"                                 { return RAISE;   }
"puts"                                  { return PUTS;    }
":="                                    { return ASSIGN;  }
"{"                                     { return LBRACE;  }
"}"                                     { return RBRACE;  }

[a-zA-Z_][a-zA-Z0-9_]*                  {
  yylval.id = malloc(strlen(yytext) + 1);
  strncpy(yylval.id, yytext, strlen(yytext) + 1);
  return ID;
}

{int}                                   {
  yylval.number = atoi(yytext);
  return CINT;
}

{float}                                 {
  yylval.fnumber = atof(yytext);
  return CFLOAT;
}

["]                                  {
  BEGIN(STR);
  yytext[0] = '\0';
}

<STR>[^"]+                            {
  yylval.string = malloc(strlen(yytext) + 1);
  strncpy(yylval.string, yytext, strlen(yytext) + 1);
}

<STR>["]                           {
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

. { jkl_error("jkl_lexer", "unknow char code %s\n", yytext); }

%%
