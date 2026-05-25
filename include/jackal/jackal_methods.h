#ifndef JACKAL_METHODS_H
#define JACKAL_METHODS_H

#include <jackal/jackal_value.h>

/*
 * Built-in native methods for the primitive types. Each has the jkl_native_fn_t
 * signature and is registered onto its class in jkl_class_init(). To add a new
 * method: implement it here with this signature and register it in
 * jkl_class_init() (see docs/types.md).
 */

/* String */
jkl_value_t jkl_str_length(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_str_upcase(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_str_downcase(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_str_reverse(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_str_concat(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_str_empty_p(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_str_to_i(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);

/* Integer */
jkl_value_t jkl_int_to_s(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_int_abs(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_int_succ(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_int_pred(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_int_even_p(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_int_odd_p(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);

/* Boolean */
jkl_value_t jkl_bool_to_s(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);
jkl_value_t jkl_bool_not(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc);

#endif
