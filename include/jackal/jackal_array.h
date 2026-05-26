#ifndef JACKAL_ARRAY_H
#define JACKAL_ARRAY_H

#include <jackal/jackal_value.h>

/*
 * Heap container for JKL_T_ARRAY values.
 *
 * Memory rule (the "trash list"):
 *   - `items[]` always holds borrowed views (every stored jkl_value_t has
 *     owned=0). Reading is cheap and safe; `jkl_array_get` returns the borrowed
 *     view directly. Callers must NOT call jkl_value_free on a value obtained
 *     from `at(i)`.
 *   - When `jkl_array_push` is handed an owned heap value (string/array/object),
 *     it MOVES the value into the `trash[]` list and stores a borrowed view at
 *     items[len]. The caller's value is then "spent" (ownership transferred,
 *     matching the existing concat/upcase convention).
 *   - Scalars / borrowed strings store as-is, nothing trashed.
 *   - `jkl_array_free` walks `trash[]` calling jkl_value_free on each, then
 *     frees the items/trash arrays and the container.
 *
 * Arrays are reference-typed semantically, but jkl_value_dup of an array is
 * FORBIDDEN in v1 (errors). Each array has exactly one owner.
 */
typedef struct {
  jkl_value_t *items;
  jkl_word_t   len;
  jkl_word_t   cap;
  jkl_value_t *trash;
  jkl_word_t   n_trash;
  jkl_word_t   trash_cap;
} jkl_array_t;

jkl_array_t *jkl_array_new(void);
void         jkl_array_free(jkl_array_t *arr);
void         jkl_array_push(jkl_array_t *arr, jkl_value_t v);
jkl_value_t  jkl_array_pop(jkl_array_t *arr);
jkl_value_t  jkl_array_get(jkl_array_t *arr, jkl_word_t i);
jkl_word_t   jkl_array_len(jkl_array_t *arr);
void         jkl_array_clear(jkl_array_t *arr);

/* Convenience: wrap an array container in an owned JKL_T_ARRAY value. */
jkl_value_t jkl_array_value(jkl_array_t *arr);

/*
 * A block (closure-less lambda). Both pointers borrow from the program AST and
 * are NOT freed by jkl_block_free / jkl_value_free — the program owns the AST.
 * Block values themselves are owned=0 and have no payload to release.
 */
typedef struct {
  jkl_node_t *params;   /* JKL_NODE_PARAMS of JKL_NODE_PARAM */
  jkl_node_t *expr;     /* the body expression */
} jkl_block_t;

#endif
