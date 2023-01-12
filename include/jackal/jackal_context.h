#ifndef JACKAL_CONTEXT_H
#define JACKAL_CONTEXT_H

#include <jackal.h>

jkl_node_t* jkl_get_context(jkl_program_t *program);
void jkl_push_context(jkl_program_t *program, jkl_node_t *node);
jkl_node_t* jkl_pop_context(jkl_program_t *program);
void jkl_ensure_empty_contexts();

#endif
