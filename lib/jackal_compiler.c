#include <jackal.h>

#define invariant(x, msg) \
  if (!(x)) \
    jkl_error("jkl_compiler", "invariant failed: " msg);

void jkl_program_init(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  program->ast_prog_root = NULL;
  program->error = 0;
}

jkl_word_t jkl_compile(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  invariant(program->ast_prog_root == NULL, "ast root is NULL");

  jkl_log("jkl_compiler", "compiling program");
  return 0;
}
