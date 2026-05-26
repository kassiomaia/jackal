#include <jackal.h>
#include <jackal/jackal_ops.h>
#include <string.h>

/* Promote either operand to float if one is float; otherwise treat both as int.
 * Bools are widened to int (0/1) for arithmetic comparisons. */
static int is_numeric(jkl_value_t v)
{
  return v.tag == JKL_T_INT || v.tag == JKL_T_FLOAT || v.tag == JKL_T_BOOL;
}

static jkl_float_t as_float(jkl_value_t v)
{
  if (v.tag == JKL_T_FLOAT) return v.as.f;
  return (jkl_float_t)v.as.i;
}

static jkl_qqword_t as_int(jkl_value_t v)
{
  if (v.tag == JKL_T_FLOAT) return (jkl_qqword_t)v.as.f;
  return v.as.i;
}

jkl_value_t jkl_apply_binop(jkl_op_t op, jkl_value_t l, jkl_value_t r)
{
  /* String concat on PLUS. */
  if (op == JKL_OP_PLUS && l.tag == JKL_T_STRING && r.tag == JKL_T_STRING) {
    jkl_word_t a = jkl_string_len(l.as.s);
    jkl_word_t b = jkl_string_len(r.as.s);
    jkl_string_t out = malloc(a + b + 1);
    if (out == NULL) jkl_error("jkl_ops", "out of memory in string concat");
    memcpy(out, l.as.s, a);
    memcpy(out + a, r.as.s, b);
    out[a + b] = '\0';
    return jkl_string_owned(out);
  }

  /* String equality. */
  if ((op == JKL_OP_EQL || op == JKL_OP_NEQ)
      && l.tag == JKL_T_STRING && r.tag == JKL_T_STRING) {
    int eq = strcmp(l.as.s, r.as.s) == 0;
    return jkl_bool(op == JKL_OP_EQL ? eq : !eq);
  }

  if (!is_numeric(l) || !is_numeric(r)) {
    jkl_error("jkl_ops", "binop on non-numeric values");
  }

  int either_float = (l.tag == JKL_T_FLOAT || r.tag == JKL_T_FLOAT);

  switch (op) {
    case JKL_OP_PLUS:
      return either_float ? jkl_float(as_float(l) + as_float(r))
                          : jkl_int(as_int(l) + as_int(r));
    case JKL_OP_MINUS:
      return either_float ? jkl_float(as_float(l) - as_float(r))
                          : jkl_int(as_int(l) - as_int(r));
    case JKL_OP_MUL:
      return either_float ? jkl_float(as_float(l) * as_float(r))
                          : jkl_int(as_int(l) * as_int(r));
    case JKL_OP_DIV:
      if (either_float) return jkl_float(as_float(l) / as_float(r));
      if (as_int(r) == 0) jkl_error("jkl_ops", "integer divide by zero");
      return jkl_int(as_int(l) / as_int(r));
    case JKL_OP_MOD:
      if (as_int(r) == 0) jkl_error("jkl_ops", "integer modulo by zero");
      return jkl_int(as_int(l) % as_int(r));
    case JKL_OP_EQL:
      return jkl_bool(either_float ? as_float(l) == as_float(r) : as_int(l) == as_int(r));
    case JKL_OP_NEQ:
      return jkl_bool(either_float ? as_float(l) != as_float(r) : as_int(l) != as_int(r));
    case JKL_OP_LT:
      return jkl_bool(either_float ? as_float(l) < as_float(r) : as_int(l) < as_int(r));
    case JKL_OP_LTE:
      return jkl_bool(either_float ? as_float(l) <= as_float(r) : as_int(l) <= as_int(r));
    case JKL_OP_GT:
      return jkl_bool(either_float ? as_float(l) > as_float(r) : as_int(l) > as_int(r));
    case JKL_OP_GTE:
      return jkl_bool(either_float ? as_float(l) >= as_float(r) : as_int(l) >= as_int(r));
    case JKL_OP_AND:
      return jkl_bool(as_int(l) && as_int(r));
    case JKL_OP_OR:
      return jkl_bool(as_int(l) || as_int(r));
    default:
      jkl_error("jkl_ops", "unsupported binop %d", op);
      return jkl_nil();
  }
}
