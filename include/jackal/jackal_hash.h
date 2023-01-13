#ifndef JACKAL_HASHOBJ_H
#define JACKAL_HASHOBJ_H

#include <jackal.h>

typedef enum {
  JKL_HASH_TYPE_STRING,
  JKL_HASH_TYPE_INT,
  JKL_HASH_TYPE_FLOAT,
  JKL_HASH_TYPE_BOOL,
  JKL_HASH_TYPE_NULL,
  JKL_HASH_TYPE_ARRAY,
  JKL_HASH_TYPE_OBJECT,
  JKL_HASH_TYPE_NIL
} jkl_hash_type_t;

typedef struct {
  char *key;
  void *data;
  jkl_hash_type_t type;
} jkl_hash_pair;

typedef struct {
  jkl_hash_pair *keys;
  unsigned int size;
  unsigned int capacity;
} jkl_hash_tbl;

jkl_hash_tbl *jkl_hash_new();
void jkl_hash_init(jkl_hash_tbl *tbl);
void jkl_hash_free(jkl_hash_tbl *tbl);
void jkl_hash_set(jkl_hash_tbl *tbl, jkl_string_t key, jkl_any_t data,
                  jkl_word_t type);
void *jkl_hash_get(jkl_hash_tbl *tbl, jkl_string_t key);
void jkl_hash_del(jkl_hash_tbl *tbl, jkl_string_t key);
void jkl_hash_to_json(jkl_hash_tbl *tbl, int depth, char *json);

#endif
