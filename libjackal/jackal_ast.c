#include <string.h>
#include <jackal/jackal_ast.h>
#include <jackal/jackal_error.h>

jkl_node_t *jkl_node_new(jkl_node_type_t type)
{
  jkl_node_t *node = malloc(sizeof(jkl_node_t));

  if (node == NULL) {
    jkl_error("jkl_ast", "failed to allocate memory");
  }

  node->type = type;
  node->node = NULL;
  node->block = NULL;
  node->block_else = NULL;
  node->parent = NULL;
  node->expr = NULL;
  node->id = NULL;
  node->assign = NULL;
  node->params = NULL;
  node->value.i = 0;
  node->value.s = NULL;
  node->value.f = 0.0;
  node->binop.left = NULL;
  node->binop.right = NULL;
  node->binop.op = JKL_OP_NONE;
  node->compound.n_nodes = 0;
  node->compound.nodes = NULL;
  return node;
}

jkl_node_t *jkl_node_binop(jkl_node_t *left, jkl_op_t op, jkl_node_t *right)
{
  jkl_node_t *binop = jkl_node_new(JKL_NODE_BINOP);
  binop->binop.left = left;
  binop->binop.op = op;
  binop->binop.right = right;
  return binop;
}

/*
 * Recursively free a node and every child it owns. `parent` is a back-pointer
 * and is never followed. Heap strings (lexer-malloc'd) live only on STRING/ID/
 * RAISE nodes. jkl_node_free(NULL) is a harmless no-op so optional children may
 * be passed unconditionally.
 */
jkl_word_t jkl_node_free(jkl_node_t *node)
{
  if (node == NULL) {
    return -1;
  }

  switch (node->type) {
    case JKL_NODE_STRING:
    case JKL_NODE_ID:
    case JKL_NODE_RAISE:
      free(node->value.s);
      break;
    case JKL_NODE_BINOP:
      jkl_node_free(node->binop.left);
      jkl_node_free(node->binop.right);
      break;
    case JKL_NODE_LET:
      jkl_node_free(node->id);
      jkl_node_free(node->expr);
      break;
    case JKL_NODE_LOOP:
      jkl_node_free(node->block);
      break;
    case JKL_NODE_IF:
      jkl_node_free(node->expr);
      jkl_node_free(node->block);
      jkl_node_free(node->block_else);
      break;
    case JKL_NODE_BLOCK:
    case JKL_NODE_PARAMS:
      for (jkl_word_t i = 0; i < node->compound.n_nodes; i++) {
        jkl_node_free(node->compound.nodes[i]);
      }
      free(node->compound.nodes);
      break;
    case JKL_NODE_PARAM:
      jkl_node_free(node->id);
      break;
    case JKL_NODE_FUNC:
      jkl_node_free(node->id);
      jkl_node_free(node->params);
      jkl_node_free(node->block);
      break;
    case JKL_NODE_CALL:
      jkl_node_free(node->id);
      jkl_node_free(node->node);
      break;
    case JKL_NODE_RETURN:
      jkl_node_free(node->expr);
      break;
    default:
      break;
  }

  free(node);
  return 0;
}

/* PRINT AST */

#define TABULATE(N)                          \
  for (jkl_word_t i = 0; i < depth + N; i++) \
    printf("  ");

void jkl_print_ast_type(jkl_node_t *node)
{
  switch (node->type) {
    case JKL_NODE_INT:
      jkl_log("jkl_ast", "JKL_NODE_INT");
      break;
    case JKL_NODE_FLOAT:
      jkl_log("jkl_ast", "JKL_NODE_FLOAT");
      break;
    case JKL_NODE_STRING:
      jkl_log("jkl_ast", "JKL_NODE_STRING");
      break;
    case JKL_NODE_ID:
      jkl_log("jkl_ast", "JKL_NODE_ID");
      break;
    case JKL_NODE_BINOP:
      jkl_log("jkl_ast", "JKL_NODE_BINOP");
      break;
    case JKL_NODE_LOOP:
      jkl_log("jkl_ast", "JKL_NODE_LOOP");
      break;
    case JKL_NODE_BLOCK:
      jkl_log("jkl_ast", "JKL_NODE_BLOCK");
      break;
    case JKL_NODE_RAISE:
      jkl_log("jkl_ast", "JKL_NODE_RAISE");
      break;
    case JKL_NODE_CALL:
      jkl_log("jkl_ast", "JKL_NODE_CALL");
      break;
    case JKL_NODE_LET:
      jkl_log("jkl_ast", "JKL_NODE_LET");
      break;
    case JKL_NODE_IF:
      jkl_log("jkl_ast", "JKL_NODE_IF");
      break;
    case JKL_NODE_FUNC:
      jkl_log("jkl_ast", "JKL_NODE_FUNC");
      break;
    case JKL_NODE_RETURN:
      jkl_log("jkl_ast", "JKL_NODE_RETURN");
      break;
    case JKL_NODE_PARAMS:
      jkl_log("jkl_ast", "JKL_NODE_PARAMS");
      break;
    case JKL_NODE_PARAM:
      jkl_log("jkl_ast", "JKL_NODE_PARAM");
      break;
    default:
      jkl_error("jkl_ast", "unknown node type");
  }
}

jkl_word_t jkl_print_ast_node(jkl_node_t *node, jkl_word_t depth)
{
  TABULATE(0);
  jkl_print_ast_type(node);

  if (node->type == JKL_NODE_ID) {
    TABULATE(1)
    printf("Value: %s\n", node->value.s);
  } else if (node->type == JKL_NODE_INT) {
  } else if (node->type == JKL_NODE_FLOAT) {
  } else if (node->type == JKL_NODE_STRING) {
    TABULATE(1)
    printf("Value: %s\n", node->value.s);
  } else if (node->type == JKL_NODE_BINOP) {
    TABULATE(1)
    switch (node->binop.op) {
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
  if (node->type == JKL_NODE_BLOCK || node->type == JKL_NODE_PARAMS) {
    jkl_log("jkl_ast", "appending to node");
    if (node->compound.nodes == NULL) {
      jkl_log("jkl_ast", "creating new node list");
      node->compound.nodes = malloc(sizeof(jkl_node_t *));
      if (node->compound.nodes == NULL) {
        jkl_error("jkl_ast", "failed to allocate memory");
        return 1;
      }
      node->compound.nodes[0] = child;
      node->compound.n_nodes = 1;
    } else {
      node->compound.n_nodes++;

      jkl_log("jkl_ast", "resize node list: %d", node->compound.n_nodes);
      jkl_log("jkl_ast", "appending to existing node list");
      node->compound.nodes = realloc(node->compound.nodes,
                                     node->compound.n_nodes * sizeof(jkl_node_t *));
      if (node->compound.nodes == NULL) {
        jkl_error("jkl_node_append", "failed to allocate memory");
        return 1;
      }
      node->compound.nodes[node->compound.n_nodes - 1] = child;
    }

    jkl_log("jkl_ast", "node appended");
    return 0;
  }

  jkl_log("jkl_ast", "ERROR: node type %d does not support children", node->type);
  exit(1);

  return 0;
}
