#include <jackal.h>

jkl_value_t jkl_nil(void)
{
  return (jkl_value_t){ .tag = JKL_T_NIL, .owned = 0, .as.i = 0 };
}

jkl_value_t jkl_int(jkl_qqword_t i)
{
  return (jkl_value_t){ .tag = JKL_T_INT, .owned = 0, .as.i = i };
}

jkl_value_t jkl_float(jkl_float_t f)
{
  return (jkl_value_t){ .tag = JKL_T_FLOAT, .owned = 0, .as.f = f };
}

jkl_value_t jkl_bool(jkl_bool_t b)
{
  return (jkl_value_t){ .tag = JKL_T_BOOL, .owned = 0, .as.i = b ? 1 : 0 };
}

jkl_value_t jkl_string(jkl_string_t s)
{
  return (jkl_value_t){ .tag = JKL_T_STRING, .owned = 0, .as.s = s };
}

jkl_value_t jkl_string_owned(jkl_string_t s)
{
  return (jkl_value_t){ .tag = JKL_T_STRING, .owned = 1, .as.s = s };
}

jkl_value_t jkl_value_dup(jkl_value_t v)
{
  if (v.tag == JKL_T_STRING && v.as.s != NULL) {
    jkl_word_t len = jkl_string_len(v.as.s);
    jkl_string_t copy = malloc(len + 1);
    if (copy == NULL) {
      jkl_error("jkl_value", "out of memory duplicating string");
    }
    memcpy(copy, v.as.s, len + 1);
    return jkl_string_owned(copy);
  }
  return v;
}

void jkl_value_free(jkl_value_t v)
{
  if (v.owned && v.tag == JKL_T_STRING && v.as.s != NULL) {
    free(v.as.s);
  }
}

jkl_string_t jkl_value_to_cstr(jkl_value_t v)
{
  char buf[64];

  switch (v.tag) {
    case JKL_T_NIL:
      return strdup("nil");
    case JKL_T_BOOL:
      return strdup(v.as.i ? "true" : "false");
    case JKL_T_INT:
      snprintf(buf, sizeof(buf), "%lld", v.as.i);
      return strdup(buf);
    case JKL_T_FLOAT:
      snprintf(buf, sizeof(buf), "%g", v.as.f);
      return strdup(buf);
    case JKL_T_STRING:
      return strdup(v.as.s ? v.as.s : "");
    default:
      return strdup("<object>");
  }
}
