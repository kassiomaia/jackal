#include <jackal.h>

void jkl_symbol_table_init(jkl_symbol_table_t *table)
{
  table->n_symbols = 0;
  for (int i = 0; i < JKL_SYMBOL_TABLE_SIZE; i++) {
    table->symbols[i].type = JKL_SYMBOL_NONE;
  }
}

jkl_word_t jkl_symbol_table_add(jkl_symbol_table_t *table, jkl_string_t id,
                                jkl_symbol_type_t type)
{

  if (jkl_symbol_table_get(table, id) != NULL) {
    jkl_error("jkl_symbol_table", "duplicated identifier %s\n", id);
  }

  jkl_word_t i = table->n_symbols;
  table->symbols[i].id = id;
  table->symbols[i].type = type;
  table->n_symbols++;

  jkl_log("jkl_symbol_table", "symbol `%s` has been added\n", id);
  return i;
}

jkl_symbol_t *jkl_symbol_table_get(jkl_symbol_table_t *table, jkl_string_t id)
{
  jkl_word_t i;
  for (i = 0; i < table->n_symbols; i++) {
    if (strcmp(table->symbols[i].id, id) == 0) {
      return &table->symbols[i];
    }
  }
  return NULL;
}

void jkl_symbol_table_free(jkl_symbol_table_t *table)
{
  free(table->symbols);
  table->n_symbols = 0;
}


