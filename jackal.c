#include <jackal.h>

extern FILE *yyin;
extern int yyparse();
extern int yylex();
extern jkl_program_t *program;

static void jkl_default_output(const char *input, char *out, size_t out_size)
{
  const char *base = strrchr(input, '/');
  base = base ? base + 1 : input;

  const char *dot = strrchr(base, '.');
  size_t len = dot ? (size_t)(dot - base) : strlen(base);

  if (len + 5 >= out_size) {       /* room for ".bin" + NUL */
    len = out_size - 5;
  }

  memcpy(out, base, len);
  strcpy(out + len, ".bin");
}

int main(int argv, char **argc)
{
  if (argv < 2) {
    fprintf(stderr, "Usage: %s <file.jkl> [out.bin]\n", argc[0]);
    exit(EXIT_FAILURE);
  }

  char out_path[256];
  if (argv >= 3) {
    snprintf(out_path, sizeof(out_path), "%s", argc[2]);
  } else {
    jkl_default_output(argc[1], out_path, sizeof(out_path));
  }

  yyin = fopen(argc[1], "r");
  if (yyin == NULL)
    jkl_error("jkl_main", "cannot open file '%s'", argc[1]);

  if (yyparse() != 0)
    jkl_error("jkl_main", "parse failed");

  fclose(yyin);

  jkl_word_t n = jkl_compile(program);
  if (n != 0)
    jkl_error("jkl_main", "compilation failed");

  jkl_ir_code_save(program->ir_code, out_path);

  jkl_ir_code_free(program->ir_code);
  free(program->ir_code);
  free(program);

  return 0;
}
