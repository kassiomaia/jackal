#include <jackal.h>

#define JKL_HASH_SIZE 1024

void jkl_hash_init(jkl_hash_tbl *tbl)
{
  tbl->capacity = JKL_HASH_SIZE;
  tbl->size = 0;
  tbl->keys = (jkl_hash_pair *)malloc(sizeof(jkl_hash_pair) * JKL_HASH_SIZE);

  if (tbl->keys == NULL)
    jkl_error("jkl_hash", "jkl_hash_init", "malloc failed");

  memset(tbl->keys, 0, sizeof(jkl_hash_pair) * tbl->capacity);
}

void jkl_hash_free(jkl_hash_tbl *tbl)
{
  tbl->capacity = 0;
  tbl->size = 0;
  free(tbl->keys);
}

void jkl_hash_set(jkl_hash_tbl *tbl, jkl_string_t key, jkl_any_t data, jkl_word_t type)
{
  if (tbl->size == tbl->capacity)
    jkl_error("jkl_hash", "jkl_hash_set", "hash table overflow");

  int hash = jkl_string_hash(key) % tbl->capacity;

  jkl_hash_pair *pair = &tbl->keys[hash];
  pair->key = key;
  pair->data = data;
  pair->type = type;

  tbl->size++;
}

void *jkl_hash_get(jkl_hash_tbl *tbl, jkl_string_t key)
{
  if (tbl->size == 0)
    jkl_error("jkl_hash", "jkl_hash_get", "hash table underflow");

  int hash = jkl_string_hash(key) % tbl->capacity;

  jkl_hash_pair *pair = &tbl->keys[hash];
  if (pair->key == NULL)
    return NULL;

  return pair->data;
}

void jkl_hash_del(jkl_hash_tbl *tbl, jkl_string_t key)
{
  if (tbl->size == 0)
    jkl_error("jkl_hash", "jkl_hash_del", "hash table underflow");

  int hash = jkl_string_hash(key) % tbl->capacity;

  jkl_hash_pair *pair = &tbl->keys[hash];
  if (pair->key == NULL)
    return;

  pair->key = NULL;
  pair->data = NULL;

  tbl->size--;
}
