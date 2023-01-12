#include <jackal.h>

#define invariant(x, msg) \
  if (x) \
    jkl_error("jkl_compiler", "invariant failed: " msg);

void jkl_program_init(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  program->ast_prog_root = NULL;
  program->error = 0;
}

jkl_word_t jkl_compile_block(jkl_program_t *program, jkl_node_t *block)
{
  invariant(program == NULL, "program is NULL");
  invariant(block == NULL, "block is NULL");
  invariant(block->type != JKL_NODE_BLOCK, "the node is not a block type");

  for (unsigned int i = 0; i < block->compound.n_nodes; i++)
  {
    jkl_node_t* child = block->compound.nodes[i];
    switch (child->type)
    {
    case JKL_NODE_LET:
      {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_LET");
        break;
      }
    case JKL_NODE_IF:
      {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_IF");
        break;
      }
    case JKL_NODE_LOOP:
      {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_LOOP");
        break;
      }
    case JKL_NODE_CALL:
      {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_CALL");
        break;
      }
    default:
      jkl_print_ast_type(child);
      jkl_error("jkl_compiler", "does not support this node element yet");
      break;
    }
  }

}

jkl_word_t jkl_compile(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  invariant(program->ast_prog_root == NULL, "ast root is NULL");

  jkl_log("jkl_compiler", "compiling program");
  jkl_compile_block(program, program->ast_prog_root);
  return 0;
}
