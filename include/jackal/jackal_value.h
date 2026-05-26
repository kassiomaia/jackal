#ifndef JACKAL_VALUE_H
#define JACKAL_VALUE_H

#include <jackal/jackal_types.h>

/*
 * Tagged runtime value. This is the representation the (future) VM and every
 * native method operate on. The `tag` selects which union member is live; for
 * STRING/ARRAY/OBJECT the `owned` flag says whether `jkl_value_free` should
 * release the heap payload (literals are borrowed; method results are owned).
 */
typedef enum {
  JKL_T_NIL,
  JKL_T_INT,
  JKL_T_FLOAT,
  JKL_T_BOOL,
  JKL_T_STRING,
  JKL_T_ARRAY,
  JKL_T_BLOCK,
  JKL_T_OBJECT,
} jkl_type_tag_t;

typedef struct {
  jkl_type_tag_t tag;
  jkl_bool_t owned;
  union {
    jkl_qqword_t i;     /* INT, and BOOL as 0/1 */
    jkl_float_t f;      /* FLOAT */
    jkl_string_t s;     /* STRING */
    jkl_any_t obj;      /* ARRAY/OBJECT (future) */
  } as;
} jkl_value_t;

jkl_value_t jkl_nil(void);
jkl_value_t jkl_int(jkl_qqword_t i);
jkl_value_t jkl_float(jkl_float_t f);
jkl_value_t jkl_bool(jkl_bool_t b);
jkl_value_t jkl_string(jkl_string_t s);         /* borrowed (owned = false) */
jkl_value_t jkl_string_owned(jkl_string_t s);   /* takes ownership */

jkl_value_t jkl_value_dup(jkl_value_t v);
void jkl_value_free(jkl_value_t v);
jkl_string_t jkl_value_to_cstr(jkl_value_t v);  /* returns owned memory */

#endif
