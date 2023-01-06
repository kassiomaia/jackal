#ifndef JACKAL_DUMP_H
#define JACKAL_DUMP_H

void jkl_symbol_table_dump(jkl_program_t *program);
void jkl_bss_dump(jkl_program_t *program);
void jkl_code_dump(jkl_program_t *program);
void jkl_dump(jkl_program_t *program);
void jkl_dump_value(jkl_value_t *value);

#endif
