#include <string.h>
#include <jackal/jackal_ast.h>
#include <jackal/jackal_dump.h>
#include <jackal/jackal_error.h>

jkl_node_t *jkl_node_new(jkl_node_type_t type)
{
  jkl_node_t *node = malloc(sizeof(jkl_node_t));

  if (node == NULL)
    jkl_error("jkl_ast", "failed to allocate memory");

  node->type = type;
  node->node = NULL;
  node->block = NULL;
  node->parent = NULL;
  node->value.i = 0;
  node->value.s = NULL;
  node->value.f = 0.0;
  node->binop.left = NULL;
  node->binop.right = NULL;
  node->binop.op = JKL_OP_NONE;
  node->unop.expr = NULL;
  node->compound.n_nodes = 0;
  node->compound.nodes = NULL;
  return node;
}

jkl_word_t jkl_node_free(jkl_node_t *node)
{
  if (node == NULL)
    return -1;

  free(node);
  return 0;
}

/* PRINT AST */

#define TABULATE(N)                          \
  for (jkl_word_t i = 0; i < depth + N; i++) \
    printf("  ");

void jkl_print_ast_type(jkl_node_t *node)
{
  switch (node->type)
  {
  case JKL_NODE_INT:
    jkl_log("jkl_ast", "JKL_NODE_INT\n");
    break;
  case JKL_NODE_FLOAT:
    jkl_log("jkl_ast", "JKL_NODE_FLOAT\n");
    break;
  case JKL_NODE_STRING:
    jkl_log("jkl_ast", "JKL_NODE_STRING\n");
    break;
  case JKL_NODE_ID:
    jkl_log("jkl_ast", "JKL_NODE_ID\n");
    break;
  case JKL_NODE_BINOP:
    jkl_log("jkl_ast", "JKL_NODE_BINOP\n");
    break;
  case JKL_NODE_LOOP:
    jkl_log("jkl_ast", "JKL_NODE_LOOP\n");
    break;
  case JKL_NODE_BLOCK:
    jkl_log("jkl_ast", "JKL_NODE_BLOCK\n");
    break;
  case JKL_NODE_RAISE:
    jkl_log("jkl_ast", "JKL_NODE_RAISE\n");
    break;
  case JKL_NODE_PUTS:
    jkl_log("jkl_ast", "JKL_NODE_PUTS\n");
    break;
  case JKL_NODE_LET:
    jkl_log("jkl_ast", "JKL_NODE_LET\n");
    break;
  default:
    jkl_error("jkl_ast", "unknown node type");
  }
}

jkl_word_t jkl_print_ast_node(jkl_node_t *node, jkl_word_t depth)
{
  TABULATE(0);
  jkl_print_ast_type(node);

  if (node->type == JKL_NODE_ID)
  {
    TABULATE(1)
    printf("Value: %s\n", node->value.s);
  }
  else if (node->type == JKL_NODE_INT)
  {
  }
  else if (node->type == JKL_NODE_FLOAT)
  {
  }
  else if (node->type == JKL_NODE_STRING)
  {
    TABULATE(1)
    printf("Value: %s\n", node->value.s);
  }
  else if (node->type == JKL_NODE_BINOP)
  {
    TABULATE(1)
    switch (node->binop.op)
    {
    case JKL_OP_ASSIGN:
      printf("ASSIGN\n");
      break;
    default:
      break;
    }
    jkl_print_ast_node(node->binop.left, depth + 2);
    jkl_print_ast_node(node->binop.right, depth + 2);
  }

  return 0;
}

jkl_word_t jkl_node_append(jkl_node_t *node, jkl_node_t *child)
{
  if (node->type == JKL_NODE_BLOCK) {
    jkl_log("jkl_node_append", "appending to block");
    if (node->compound.nodes == NULL) {
      jkl_log("jkl_node_append", "creating new node list");
      node->compound.nodes = malloc(sizeof(jkl_node_t*));
      if (node->compound.nodes == NULL) {
        jkl_error("jkl_node_append", "failed to allocate memory");
        return 1;
      }
      node->compound.nodes[0] = child;
      node->compound.n_nodes = 1;
    } else {
      node->compound.n_nodes++;

      jkl_log("jkl_node_append", "resize node list: %d", node->compound.n_nodes);
      jkl_log("jkl_node_append", "appending to existing node list");
      node->compound.nodes = realloc(node->compound.nodes, node->compound.n_nodes * sizeof(jkl_node_t*));
      if (node->compound.nodes == NULL) {
        jkl_error("jkl_node_append", "failed to allocate memory");
        return 1;
      }
      node->compound.nodes[node->compound.n_nodes - 1] = child;
    }

    jkl_log("jkl_node_append", "node appended");
    return 0;
  }

  jkl_log("jkl_node_append", "ERROR: node type %d does not support children", node->type);
  exit(1);

  return 0;
}
