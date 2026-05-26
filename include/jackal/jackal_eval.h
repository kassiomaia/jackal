#ifndef JACKAL_EVAL_H
#define JACKAL_EVAL_H

#include <jackal/jackal_types.h>
#include <jackal/jackal_value.h>
#include <jackal/jackal_compiler.h>

/* A tiny parameter environment for block evaluation: linear array of
 * (name -> value) lookups. Param binding only — no closures in v1. */
typedef struct {
  jkl_string_t *names;
  jkl_value_t  *vals;
  jkl_word_t    n;
} jkl_env_t;

/*
 * Tree-walking expression evaluator. The scope is intentionally narrow: enough
 * to run block bodies (literals, identifiers, binops, method calls, nested
 * array/block literals). Does NOT support statements, let, or control flow.
 * Caller frees the returned value (owned/borrowed per the existing convention).
 */
jkl_value_t jkl_eval_expr(jkl_node_t *expr, jkl_env_t *env);

/*
 * Invoke a block value (JKL_T_BLOCK) with argc positional arguments. Binds them
 * to the block's formal params and evaluates its single-expression body.
 */
jkl_value_t jkl_block_call(jkl_value_t block, jkl_value_t *argv, jkl_word_t argc);

/* Legacy stub kept for API compatibility (no-op; returns 0). */
jkl_word_t jkl_eval(jkl_program_t *program);

#endif
