#include <jackal.h>

#define invariant(x, msg) \
  if (x) \
    jkl_error("jlk_eval", "invariant failed: " msg);

jkl_word_t jkl_eval(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  invariant(program->ast_prog_root == NULL, "ast root is NULL");
  invariant(program->error != 0, "program has error");
  invariant(program->ast_prog_root->type != JKL_NODE_BLOCK, "ast root is not block");

  jkl_log("jkl_eval", "evaluating program");

  return 0;
}
