#include <jackal.h>
#include <jackal/jackal_array.h>

static void jkl_array_items_grow(jkl_array_t *arr)
{
  arr->cap = arr->cap ? arr->cap * 2 : 4;
  arr->items = realloc(arr->items, arr->cap * sizeof(jkl_value_t));
  if (arr->items == NULL) {
    jkl_error("jkl_array", "out of memory growing items");
  }
}

static void jkl_array_trash_push(jkl_array_t *arr, jkl_value_t v)
{
  if (arr->n_trash == arr->trash_cap) {
    arr->trash_cap = arr->trash_cap ? arr->trash_cap * 2 : 4;
    arr->trash = realloc(arr->trash, arr->trash_cap * sizeof(jkl_value_t));
    if (arr->trash == NULL) {
      jkl_error("jkl_array", "out of memory growing trash");
    }
  }
  arr->trash[arr->n_trash++] = v;
}

jkl_array_t *jkl_array_new(void)
{
  jkl_array_t *arr = calloc(1, sizeof(jkl_array_t));
  if (arr == NULL) {
    jkl_error("jkl_array", "out of memory");
  }
  return arr;
}

void jkl_array_free(jkl_array_t *arr)
{
  if (arr == NULL) {
    return;
  }
  for (jkl_word_t i = 0; i < arr->n_trash; i++) {
    jkl_value_free(arr->trash[i]);
  }
  free(arr->items);
  free(arr->trash);
  free(arr);
}

/* Does this value own a heap payload (so we must move it to trash)? */
static int value_owns_heap(jkl_value_t v)
{
  if (!v.owned) {
    return 0;
  }
  return v.tag == JKL_T_STRING || v.tag == JKL_T_ARRAY || v.tag == JKL_T_OBJECT;
}

void jkl_array_push(jkl_array_t *arr, jkl_value_t v)
{
  if (arr->len == arr->cap) {
    jkl_array_items_grow(arr);
  }
  if (value_owns_heap(v)) {
    /* Move into trash; store borrowed view in items. */
    jkl_array_trash_push(arr, v);
    jkl_value_t borrowed = v;
    borrowed.owned = 0;
    arr->items[arr->len++] = borrowed;
  } else {
    arr->items[arr->len++] = v;
  }
}

/*
 * Pop the last element. If it had a corresponding trash entry, transfer
 * ownership back to the returned value (the caller is now responsible for
 * freeing it). Other elements stay in their (LIFO) trash position.
 */
jkl_value_t jkl_array_pop(jkl_array_t *arr)
{
  if (arr->len == 0) {
    return jkl_nil();
  }
  jkl_value_t v = arr->items[--arr->len];
  /* If the value we just popped matches the top of trash (string/array/object),
   * transfer ownership back to the caller. */
  if ((v.tag == JKL_T_STRING || v.tag == JKL_T_ARRAY || v.tag == JKL_T_OBJECT)
      && arr->n_trash > 0
      && arr->trash[arr->n_trash - 1].tag == v.tag
      && arr->trash[arr->n_trash - 1].as.s == v.as.s) {
    arr->n_trash--;
    return arr->trash[arr->n_trash];     /* owned=1 */
  }
  return v;
}

jkl_value_t jkl_array_get(jkl_array_t *arr, jkl_word_t i)
{
  if (i < 0 || i >= arr->len) {
    return jkl_nil();
  }
  return arr->items[i];   /* borrowed */
}

jkl_word_t jkl_array_len(jkl_array_t *arr)
{
  return arr->len;
}

void jkl_array_clear(jkl_array_t *arr)
{
  for (jkl_word_t i = 0; i < arr->n_trash; i++) {
    jkl_value_free(arr->trash[i]);
  }
  arr->n_trash = 0;
  arr->len = 0;
}

jkl_value_t jkl_array_value(jkl_array_t *arr)
{
  jkl_value_t v;
  v.tag = JKL_T_ARRAY;
  v.owned = 1;
  v.as.obj = arr;
  return v;
}
