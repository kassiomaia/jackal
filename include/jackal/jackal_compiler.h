#ifndef JACKAL_COMPILER_H
#define JACKAL_COMPILER_H

#include <jackal/jackal_types.h>
#include <jackal/jackal_symbol_table.h>
#include <jackal/jackal_ir.h>

typedef struct {
  jkl_word_t idx;
  jkl_string_t name;
} jkl_call_fixup_t;

typedef struct {
  jkl_node_t *ast_prog_root;
  jkl_error_t n_errors;
  jkl_word_t n_warnings;
  jkl_word_t n_notes;
  jkl_word_t n_ids;
  jkl_word_t n_funcs;
  jkl_symbol_table_t *symbol_table;
  jkl_ir_code_t *ir_code;
  jkl_node_t **func_queue;
  size_t n_func_queue;
  size_t func_queue_cap;
  jkl_call_fixup_t *call_fixups;
  size_t n_fixups;
  size_t fixups_cap;
} jkl_program_t;

/*
 * Compiler
 */

jkl_program_t *jkl_program_new();

void jkl_program_init(jkl_program_t *program);

void jkl_program_free(jkl_program_t *program);

jkl_word_t jkl_compile(jkl_program_t *program);

#endif
