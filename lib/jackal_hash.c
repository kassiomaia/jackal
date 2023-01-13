#include <jackal.h>

#define JKL_HASH_SIZE 1024

void jkl_free_pair(jkl_hash_pair* pair);

jkl_hash_tbl* jkl_hash_new()
{
  jkl_hash_tbl *instance = malloc(sizeof(jkl_hash_tbl));
  if (instance == NULL)
    jkl_error("jkl_hash", "jkl_hash_new: malloc failed");
  
  memset(instance, 0, sizeof(jkl_hash_tbl));
  jkl_hash_init(instance);
  
  return instance;
}

void jkl_hash_init(jkl_hash_tbl *tbl)
{
  tbl->capacity = JKL_HASH_SIZE;
  tbl->size = 0;
  tbl->keys = malloc(sizeof(jkl_hash_pair) * JKL_HASH_SIZE);

  if (tbl->keys == NULL)
    jkl_error("jkl_hash", "jkl_hash_init: malloc failed");

  memset(tbl->keys, 0, sizeof(jkl_hash_pair) * tbl->capacity);
}

void jkl_hash_free(jkl_hash_tbl *tbl)
{
  tbl->capacity = 0;
  tbl->size = 0;
  
  for (int i = 0; i < tbl->capacity; i++)
  {
    jkl_hash_pair* child = &tbl->keys[i];
    if (child->key != NULL)
      jkl_free_pair(child);
  }
  
  free(tbl->keys);
}

void jkl_hash_set(jkl_hash_tbl *tbl, jkl_string_t key, jkl_any_t data, jkl_word_t type)
{
  if (tbl->size == tbl->capacity)
    jkl_error("jkl_hash", "jkl_hash_set: hash table overflow");

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
    jkl_error("jkl_hash", "jkl_hash_get: hash table underflow");

  int hash = jkl_string_hash(key) % tbl->capacity;

  jkl_hash_pair *pair = &tbl->keys[hash];
  if (pair->key == NULL)
    return NULL;

  return pair->data;
}

void jkl_free_pair(jkl_hash_pair* pair)
{
  if (pair->type == JKL_HASH_TYPE_OBJECT)
  {
    jkl_hash_tbl *tbl = (jkl_hash_tbl*)pair->data;
    for (int i = 0; i < tbl->capacity; i++)
    {
      jkl_hash_pair* child = &tbl->keys[i];
      if (child->key != NULL)
        jkl_free_pair(child);
    }
    free(tbl);
  } else {
    free(pair->key);
    free(pair->data);
  }
}

void jkl_hash_del(jkl_hash_tbl *tbl, jkl_string_t key)
{
  if (tbl->size == 0)
    jkl_error("jkl_hash", "jkl_hash_del: hash table underflow");

  int hash = jkl_string_hash(key) % tbl->capacity;

  jkl_hash_pair *pair = &tbl->keys[hash];
  if (pair->key == NULL)
    return;

  jkl_free_pair(pair);
  
  pair->key = NULL;
  pair->data = NULL;

  tbl->size--;
}

#define jsoncat(buf, d_, v_) \
  do { \
    for (int i = 0; i < d_; i++) strcat(buf, "  "); \
    strcat(buf, v_); \
  } while (0);

void jkl_hash_to_json(jkl_hash_tbl *tbl, int depth, char* json)
{
  if (tbl == NULL)
    return NULL;
  if (tbl->size == 0)
    return NULL;

  jsoncat(json, depth, "{\n");

  for (int i = 0; i < tbl->capacity; i++)
  {
    if (tbl->keys[i].key == NULL)
      continue;

    jsoncat(json, depth + 1, "\"");
    jsoncat(json, 0, tbl->keys[i].key);
    jsoncat(json, 0, "\": ");

    switch (tbl->keys[i].type)
    {
    case JKL_HASH_TYPE_STRING:
      jsoncat(json, 0, "\"");
      jsoncat(json, 0, tbl->keys[i].data);
      jsoncat(json, 0, "\"");
      break;
    case JKL_HASH_TYPE_INT:
      jsoncat(json, 0, (int*)tbl->keys[i].data);
      break;
    case JKL_HASH_TYPE_FLOAT:
      jsoncat(json, 0, (float*)tbl->keys[i].data);
      break;
    case JKL_HASH_TYPE_BOOL:
      jsoncat(json, 0, (bool*)tbl->keys[i].data);
      break;
    case JKL_HASH_TYPE_NIL:
      jsoncat(json, 0, "null");
      break;
    case JKL_HASH_TYPE_OBJECT:
      jkl_hash_to_json((jkl_hash_tbl*)tbl->keys[i].data, depth + 1, json);
      break;
    default:
      break;
    }
    jsoncat(json, 0, ",\n");
  }
  jsoncat(json, depth, "}");
}
