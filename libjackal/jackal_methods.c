#include <jackal.h>
#include <jackal/jackal_methods.h>
#include <ctype.h>
#include <stdlib.h>

/*
 * String methods (self.tag == JKL_T_STRING, payload in self.as.s)
 */

jkl_value_t jkl_str_length(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_int(jkl_string_len(self.as.s));
}

static jkl_value_t str_map_case(jkl_value_t self, int upper)
{
  jkl_word_t len = jkl_string_len(self.as.s);
  jkl_string_t out = malloc(len + 1);
  if (out == NULL) {
    jkl_error("jkl_methods", "out of memory");
  }
  for (jkl_word_t i = 0; i < len; i++) {
    out[i] = upper ? toupper((unsigned char)self.as.s[i])
                   : tolower((unsigned char)self.as.s[i]);
  }
  out[len] = '\0';
  return jkl_string_owned(out);
}

jkl_value_t jkl_str_upcase(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return str_map_case(self, 1);
}

jkl_value_t jkl_str_downcase(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return str_map_case(self, 0);
}

jkl_value_t jkl_str_reverse(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  jkl_word_t len = jkl_string_len(self.as.s);
  jkl_string_t out = malloc(len + 1);
  if (out == NULL) {
    jkl_error("jkl_methods", "out of memory");
  }
  for (jkl_word_t i = 0; i < len; i++) {
    out[i] = self.as.s[len - 1 - i];
  }
  out[len] = '\0';
  return jkl_string_owned(out);
}

jkl_value_t jkl_str_concat(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 1 || argv[0].tag != JKL_T_STRING) {
    jkl_error("jkl_methods", "concat expects a string argument");
  }
  jkl_word_t a = jkl_string_len(self.as.s);
  jkl_word_t b = jkl_string_len(argv[0].as.s);
  jkl_string_t out = malloc(a + b + 1);
  if (out == NULL) {
    jkl_error("jkl_methods", "out of memory");
  }
  memcpy(out, self.as.s, a);
  memcpy(out + a, argv[0].as.s, b);
  out[a + b] = '\0';
  return jkl_string_owned(out);
}

jkl_value_t jkl_str_empty_p(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_bool(jkl_string_len(self.as.s) == 0);
}

jkl_value_t jkl_str_to_i(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_int(atoll(self.as.s));
}

/*
 * Integer methods (self.as.i)
 */

jkl_value_t jkl_int_to_s(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_string_owned(jkl_value_to_cstr(self));
}

jkl_value_t jkl_int_abs(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_int(self.as.i < 0 ? -self.as.i : self.as.i);
}

jkl_value_t jkl_int_succ(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_int(self.as.i + 1);
}

jkl_value_t jkl_int_pred(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_int(self.as.i - 1);
}

jkl_value_t jkl_int_even_p(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_bool((self.as.i % 2) == 0);
}

jkl_value_t jkl_int_odd_p(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_bool((self.as.i % 2) != 0);
}

/*
 * Boolean methods (self.as.i is 0/1)
 */

jkl_value_t jkl_bool_to_s(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_string_owned(jkl_value_to_cstr(self));
}

jkl_value_t jkl_bool_not(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv;
  (void)argc;
  return jkl_bool(!self.as.i);
}
