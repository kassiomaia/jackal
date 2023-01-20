#include <jackal.h>

jkl_word_t jkl_is_primitive(jkl_node_t *node)
{
  switch (node->type) {
    case JKL_NODE_INT:
    case JKL_NODE_STRING:
    case JKL_NODE_FLOAT:
      return 1;
    default:
      return 0;
  }
}

void jkl_optimize_expr(jkl_node_t *node)
{
  jkl_note("jkl_optimizer", "optimizing node expression");
  if (node->type == JKL_NODE_BINOP) {
    jkl_note("jkl_optimizer", "this is a binary operation");
    jkl_node_t *lhs = node->binop.left;
    jkl_node_t *rhs = node->binop.right;
    jkl_word_t r = jkl_is_primitive(lhs) && jkl_is_primitive(rhs);
    if (r == 1) {
      jkl_note("jkl_optimizer", "lhs and rhs are primitive types");
      if (lhs->type != rhs->type) {
        jkl_error("jkl_optimizer", "lhs and rhs are not of the same type");
      }
    }
  }
}
