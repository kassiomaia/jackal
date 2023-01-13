#ifndef JACKAL_SYMBOL_TABLE_H
#define JACKAL_SYMBOL_TABLE_H

#include <jackal.h>

#define JKL_SYMBOL_TABLE_SIZE 1024

typedef enum {
  JKL_SYMBOL_NONE,
  JKL_SYMBOL_LET,
  JKL_SYMBOL_FUNCTION,
  JKL_SYMBOL_CLASS,
  JKL_SYMBOL_CLASS_METHOD,
} jkl_symbol_type_t;

typedef struct {
  jkl_string_t id;
  jkl_symbol_type_t type;
} jkl_symbol_t;

typedef struct {
  jkl_symbol_t symbols[JKL_SYMBOL_TABLE_SIZE];
  size_t n_symbols;
} jkl_symbol_table_t;

void jkl_symbol_table_init(jkl_symbol_table_t *table);
jkl_word_t jkl_symbol_table_add(jkl_symbol_table_t *table, jkl_string_t id,
                                jkl_symbol_type_t type);
jkl_symbol_t *jkl_symbol_table_get(jkl_symbol_table_t *table, jkl_string_t id);
void jkl_symbol_table_free(jkl_symbol_table_t *table);

#endif
