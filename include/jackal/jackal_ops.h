#ifndef JACKAL_OPS_H
#define JACKAL_OPS_H

#include <jackal/jackal_value.h>
#include <jackal/jackal_types.h>

/*
 * Apply a binary operator to two values. Used by the evaluator today (for block
 * bodies) and by the future VM (for ADD/SUB/... IR opcodes). The result is an
 * owned-or-borrowed value following the existing convention (concat returns
 * owned strings; numerics return scalars).
 */
jkl_value_t jkl_apply_binop(jkl_op_t op, jkl_value_t l, jkl_value_t r);

#endif
