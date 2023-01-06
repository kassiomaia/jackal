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
  {
    fprintf(stderr, "Error: Cannot open file '%s'\n", argc[1]);
    exit(EXIT_FAILURE);
  }

  yyparse();
  fclose(yyin);
  return 0;
}
