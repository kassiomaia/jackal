#include <jackal.h>

extern FILE *yyin;
extern int yyparse();
extern int yylex();

int main(int argv, char **argc)
{
  if (argv < 2)
  {
    fprintf(stderr, "Usage: %s <file>\n", argc[0]);
    exit(EXIT_FAILURE);
  }

  yyin = fopen(argc[1], "r");
  if (yyin == NULL)
    jkl_error("jkl_main", "cannot open file '%s'", argc[1]);

  yyparse();
  fclose(yyin);

  return 0;
}
