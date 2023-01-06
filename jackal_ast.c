#include <jackal/jackal_ast.h>
#include <jackal/jackal_dump.h>

#define TABULATE(N)                          \
  for (jkl_word_t i = 0; i < depth + N; i++) \
    printf("  ");

void jkl_print_ast_type(jkl_node_t *node)
{
  switch (node->type)
  {
  case JKL_NODE_ID:
    printf("ID\n");
    break;
  case JKL_NODE_BINOP:
    printf("BINOP\n");
    break;
  case JKL_NODE_INT:
    printf("INT\n");
    break;
  case JKL_NODE_FLOAT:
    printf("FLOAT\n");
    break;
  case JKL_NODE_STRING:
    printf("STRING\n");
    break;
  default:
    printf("UNKNOWN\n");
    break;
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
    printf("[jkl_node_append] appending to block\n");
    if (node->compound.nodes == NULL) {
      printf("[jkl_node_append] creating new node list\n");
      node->compound.nodes = calloc(1, sizeof(jkl_node_t));
      node->compound.nodes[0] = *child;
      node->compound.n_nodes = 1;
      printf("[jkl_node_append] new node list created\n");
    } else {
      printf("[jkl_node_append] appending to existing node list\n");
      node->compound.n_nodes++;
      printf("[jkl_node_append] new node list size: %d\n", node->compound.n_nodes);
      node->compound.nodes = realloc(node->compound.nodes, node->compound.n_nodes * sizeof(jkl_node_t));
      node->compound.nodes[node->compound.n_nodes - 1] = *child;
      printf("[jkl_node_append] node appended\n");
    }

    printf("[jkl_node_append] node appended\n");
    return 0;
  }

  printf("[jkl_node_append] ERROR: node type %d does not support children\n", node->type);
  exit(1);

  return 0;
}
