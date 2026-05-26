#include <jackal.h>
#include <jackal/jackal_array.h>
#include <jackal/jackal_methods.h>
#include <stdlib.h>
#include <string.h>

/* The native iterator methods live below the data-manipulation ones; they call
 * jkl_block_call (declared in jackal_eval.h). */
#include <jackal/jackal_eval.h>

static jkl_array_t *as_array(jkl_value_t self)
{
  if (self.tag != JKL_T_ARRAY || self.as.obj == NULL) {
    jkl_error("jkl_array_methods", "receiver is not an array");
  }
  return (jkl_array_t *)self.as.obj;
}

static jkl_value_t borrow_self(jkl_value_t self)
{
  jkl_value_t v = self;
  v.owned = 0;
  return v;
}

/*
 * Data-manipulation methods
 */

jkl_value_t jkl_arr_length(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv; (void)argc;
  return jkl_int(jkl_array_len(as_array(self)));
}

jkl_value_t jkl_arr_at(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 1 || argv[0].tag != JKL_T_INT) {
    jkl_error("jkl_array_methods", "at expects an integer index");
  }
  return jkl_array_get(as_array(self), (jkl_word_t)argv[0].as.i);
}

jkl_value_t jkl_arr_push(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 1) {
    jkl_error("jkl_array_methods", "push expects an argument");
  }
  jkl_array_push(as_array(self), argv[0]);
  return borrow_self(self);
}

jkl_value_t jkl_arr_pop(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv; (void)argc;
  return jkl_array_pop(as_array(self));
}

jkl_value_t jkl_arr_clear(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv; (void)argc;
  jkl_array_clear(as_array(self));
  return borrow_self(self);
}

jkl_value_t jkl_arr_empty_p(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv; (void)argc;
  return jkl_bool(jkl_array_len(as_array(self)) == 0);
}

/* Build a new array containing the source's elements in reverse. Strings are
 * deep-copied so the result is independent; nested arrays/blocks are forbidden
 * (jkl_value_dup errors). */
jkl_value_t jkl_arr_reverse(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  (void)argv; (void)argc;
  jkl_array_t *src = as_array(self);
  jkl_array_t *dst = jkl_array_new();

  for (jkl_word_t i = src->len - 1; i >= 0; i--) {
    jkl_value_t e = src->items[i];
    if (e.tag == JKL_T_STRING && e.as.s != NULL) {
      e = jkl_value_dup(e); /* owned copy */
    }
    jkl_array_push(dst, e); /* moves the owned string into dst's trash */
  }
  return jkl_array_value(dst);
}

/* join(sep): concatenate elements' string forms separated by sep. Returns an
 * owned string. */
jkl_value_t jkl_arr_join(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 1 || argv[0].tag != JKL_T_STRING) {
    jkl_error("jkl_array_methods", "join expects a string separator");
  }
  jkl_array_t *arr = as_array(self);
  jkl_string_t sep = argv[0].as.s;
  jkl_word_t sep_len = jkl_string_len(sep);

  /* Stringify each element into a heap buffer; track total length. */
  jkl_word_t total = 0;
  jkl_string_t *parts = arr->len > 0 ? calloc(arr->len, sizeof(jkl_string_t)) : NULL;
  for (jkl_word_t i = 0; i < arr->len; i++) {
    parts[i] = jkl_value_to_cstr(arr->items[i]); /* owned */
    total += jkl_string_len(parts[i]);
    if (i + 1 < arr->len) total += sep_len;
  }

  jkl_string_t out = malloc(total + 1);
  if (out == NULL) {
    jkl_error("jkl_array_methods", "out of memory in join");
  }
  jkl_word_t off = 0;
  for (jkl_word_t i = 0; i < arr->len; i++) {
    jkl_word_t l = jkl_string_len(parts[i]);
    memcpy(out + off, parts[i], l);
    off += l;
    if (i + 1 < arr->len) {
      memcpy(out + off, sep, sep_len);
      off += sep_len;
    }
    free(parts[i]);
  }
  out[total] = '\0';
  free(parts);
  return jkl_string_owned(out);
}

/*
 * Iterators — drive the block evaluator (jkl_block_call). Element views are
 * borrowed; block results are owned (caller's responsibility to free or hand off
 * via push).
 */

jkl_value_t jkl_arr_each(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 1 || argv[0].tag != JKL_T_BLOCK) {
    jkl_error("jkl_array_methods", "each expects a block");
  }
  jkl_array_t *arr = as_array(self);
  for (jkl_word_t i = 0; i < arr->len; i++) {
    jkl_value_t elem = arr->items[i];
    jkl_value_t r = jkl_block_call(argv[0], &elem, 1);
    jkl_value_free(r);
  }
  return borrow_self(self);
}

jkl_value_t jkl_arr_map(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 1 || argv[0].tag != JKL_T_BLOCK) {
    jkl_error("jkl_array_methods", "map expects a block");
  }
  jkl_array_t *arr = as_array(self);
  jkl_array_t *out = jkl_array_new();
  for (jkl_word_t i = 0; i < arr->len; i++) {
    jkl_value_t elem = arr->items[i];
    jkl_value_t r = jkl_block_call(argv[0], &elem, 1);
    jkl_array_push(out, r); /* moves owned results into trash */
  }
  return jkl_array_value(out);
}

jkl_value_t jkl_arr_filter(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 1 || argv[0].tag != JKL_T_BLOCK) {
    jkl_error("jkl_array_methods", "filter expects a block");
  }
  jkl_array_t *arr = as_array(self);
  jkl_array_t *out = jkl_array_new();
  for (jkl_word_t i = 0; i < arr->len; i++) {
    jkl_value_t elem = arr->items[i];
    jkl_value_t r = jkl_block_call(argv[0], &elem, 1);
    int keep = (r.tag == JKL_T_BOOL && r.as.i) || (r.tag == JKL_T_INT && r.as.i);
    jkl_value_free(r);
    if (keep) {
      /* Push the borrowed view; the result array holds non-owning aliases of
       * the source. The source must outlive the filtered result. */
      jkl_array_push(out, elem);
    }
  }
  return jkl_array_value(out);
}

/* reduce(init, blk): free previous acc before replacing with the block's result
 * — the canonical "iterator with state" pattern. */
jkl_value_t jkl_arr_reduce(jkl_value_t self, jkl_value_t *argv, jkl_word_t argc)
{
  if (argc < 2 || argv[1].tag != JKL_T_BLOCK) {
    jkl_error("jkl_array_methods", "reduce expects (init, block)");
  }
  jkl_array_t *arr = as_array(self);
  jkl_value_t acc = argv[0];                  /* borrowed unless caller passed owned */
  int acc_owned_by_us = 0;
  for (jkl_word_t i = 0; i < arr->len; i++) {
    jkl_value_t pair[2] = { acc, arr->items[i] };
    jkl_value_t next = jkl_block_call(argv[1], pair, 2);
    if (acc_owned_by_us) {
      jkl_value_free(acc);
    }
    acc = next;
    acc_owned_by_us = 1;
  }
  return acc;
}
