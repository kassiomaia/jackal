#ifndef JACKAL_COMPILER_H
#define JACKAL_COMPILER_H

#include <stdlib.h>
#include <stdio.h>
#include "jackal_types.h"

/*
 * Compiler
 */

void jkl_program_init(jkl_program_t *program);

jkl_word_t jkl_emit_symbol(jkl_program_t *program, jkl_string_t symbol);
jkl_word_t jkl_emit_const_bool(jkl_program_t *program, jkl_bool_t value);
jkl_word_t jkl_emit_const_char(jkl_program_t *program, jkl_char_t value);
jkl_word_t jkl_emit_const_byte(jkl_program_t *program, jkl_byte_t value);
jkl_word_t jkl_emit_const_word(jkl_program_t *program, jkl_word_t value);
jkl_word_t jkl_emit_const_dword(jkl_program_t *program, jkl_dword_t value);
jkl_word_t jkl_emit_const_qword(jkl_program_t *program, jkl_qword_t value);
jkl_word_t jkl_emit_const_qqword(jkl_program_t *program, jkl_qqword_t value);
jkl_word_t jkl_emit_const_float(jkl_program_t *program, jkl_float_t value);
jkl_word_t jkl_emit_const_double(jkl_program_t *program, jkl_double_t value);
jkl_word_t jkl_emit_const_ldouble(jkl_program_t *program, jkl_ldouble_t value);
jkl_word_t jkl_emit_const_string(jkl_program_t *program, jkl_string_t value);
jkl_word_t jkl_emit_const_pointer(jkl_program_t *program, jkl_pointer_t value);
jkl_word_t jkl_emit_const_function(jkl_program_t *program, jkl_function_t value);
jkl_word_t jkl_emit_inst_0(jkl_program_t *program, jkl_inst_type_t type);
jkl_word_t jkl_emit_inst_1(jkl_program_t *program, jkl_inst_type_t type, jkl_word_t lhs);
jkl_word_t jkl_emit_inst_2(jkl_program_t *program, jkl_inst_type_t type, jkl_word_t lhs, jkl_word_t rhs);
jkl_word_t jkl_symbol_load(jkl_program_t *program, jkl_string_t symbol);
jkl_bool_t jkl_symbol_exist(jkl_program_t *program, jkl_string_t symbol);
jkl_bool_t jkl_get_symbol_idx_exist(jkl_program_t *program, jkl_word_t hash);
jkl_bool_t jkl_get_symbol(jkl_program_t *program, jkl_word_t hash, jkl_string_t *symbol);
jkl_word_t jkl_emit_binop(jkl_program_t *program, jkl_node_t *node);
jkl_word_t jkl_emit_loop(jkl_program_t *program, jkl_node_t *node);
jkl_word_t jkl_emit_block(jkl_program_t *program, jkl_node_t *node);
jkl_word_t jkl_emit_raise(jkl_program_t *program, jkl_node_t *node);
jkl_word_t jkl_emit_puts(jkl_program_t *program, jkl_node_t *node);

#endif
