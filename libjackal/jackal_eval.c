#include <jackal.h>
#include <jackal/jackal_array.h>
#include <jackal/jackal_eval.h>
#include <jackal/jackal_ops.h>
#include <string.h>

#define JKL_BLOCK_MAX_PARAMS 8

static jkl_value_t env_lookup(jkl_env_t *env, jkl_string_t name)
{
  if (env == NULL) {
    jkl_error("jkl_eval", "no environment when looking up '%s'", name);
  }
  for (jkl_word_t i = 0; i < env->n; i++) {
    if (strcmp(env->names[i], name) == 0) {
      return env->vals[i];
    }
  }
  jkl_error("jkl_eval", "undefined identifier '%s' in block body", name);
  return jkl_nil();
}

jkl_value_t jkl_eval_expr(jkl_node_t *expr, jkl_env_t *env)
{
  if (expr == NULL) {
    jkl_error("jkl_eval", "expr is NULL");
  }

  switch (expr->type) {
    case JKL_NODE_INT:    return jkl_int(expr->value.i);
    case JKL_NODE_FLOAT:  return jkl_float(expr->value.f);
    case JKL_NODE_BOOL:   return jkl_bool(expr->value.i ? 1 : 0);
    case JKL_NODE_STRING: return jkl_string(expr->value.s);  /* borrowed AST string */
    case JKL_NODE_ID:     return env_lookup(env, expr->value.s);

    case JKL_NODE_BINOP: {
      jkl_value_t l = jkl_eval_expr(expr->binop.left, env);
      jkl_value_t r = jkl_eval_expr(expr->binop.right, env);
      jkl_value_t result = jkl_apply_binop(expr->binop.op, l, r);
      jkl_value_free(l);
      jkl_value_free(r);
      return result;
    }

    case JKL_NODE_METHOD_CALL: {
      jkl_value_t recv = jkl_eval_expr(expr->node, env);
      jkl_word_t argc = expr->params ? expr->params->compound.n_nodes : 0;
      jkl_value_t argv[JKL_BLOCK_MAX_PARAMS];
      if (argc > JKL_BLOCK_MAX_PARAMS) {
        jkl_error("jkl_eval", "too many args in block-body method call");
      }
      for (jkl_word_t i = 0; i < argc; i++) {
        argv[i] = jkl_eval_expr(expr->params->compound.nodes[i], env);
      }
      jkl_value_t result = jkl_send(recv, expr->id->value.s, argv, argc);
      for (jkl_word_t i = 0; i < argc; i++) {
        jkl_value_free(argv[i]);
      }
      jkl_value_free(recv);
      return result;
    }

    case JKL_NODE_ARRAY_LIT: {
      jkl_array_t *arr = jkl_array_new();
      for (jkl_word_t i = 0; i < expr->compound.n_nodes; i++) {
        jkl_value_t e = jkl_eval_expr(expr->compound.nodes[i], env);
        jkl_array_push(arr, e);
      }
      return jkl_array_value(arr);
    }

    default:
      jkl_error("jkl_eval", "unsupported node type %d in block body", expr->type);
      return jkl_nil();
  }
}

jkl_value_t jkl_block_call(jkl_value_t block, jkl_value_t *argv, jkl_word_t argc)
{
  if (block.tag != JKL_T_BLOCK || block.as.obj == NULL) {
    jkl_error("jkl_eval", "jkl_block_call: not a block value");
  }
  jkl_block_t *blk = (jkl_block_t *)block.as.obj;
  jkl_node_t *params = blk->params;
  jkl_word_t n_params = params ? params->compound.n_nodes : 0;

  if (n_params > JKL_BLOCK_MAX_PARAMS) {
    jkl_error("jkl_eval", "block has too many params (max %d)",
              JKL_BLOCK_MAX_PARAMS);
  }
  if (argc < n_params) {
    jkl_error("jkl_eval", "block expects %d arg(s), got %d", n_params, argc);
  }

  jkl_string_t names[JKL_BLOCK_MAX_PARAMS];
  jkl_value_t  vals[JKL_BLOCK_MAX_PARAMS];
  for (jkl_word_t i = 0; i < n_params; i++) {
    names[i] = params->compound.nodes[i]->id->value.s;
    vals[i] = argv[i];
  }
  jkl_env_t env = { .names = names, .vals = vals, .n = n_params };
  return jkl_eval_expr(blk->expr, &env);
}

/* Legacy no-op kept so older callers/headers don't break. The real evaluator
 * works on block bodies, not whole programs. */
jkl_word_t jkl_eval(jkl_program_t *program)
{
  (void)program;
  return 0;
}
