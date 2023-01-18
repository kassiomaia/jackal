#ifndef JACKAL_AST_H
#define JACKAL_AST_H

#include <jackal/jackal_types.h>

/*
 * AST
 */

jkl_node_t *jkl_node_new(jkl_node_type_t type);
jkl_word_t jkl_node_free(jkl_node_t *node);
jkl_word_t jkl_node_append(jkl_node_t *node, jkl_node_t *child);
jkl_word_t jkl_print_ast_node(jkl_node_t *node, jkl_word_t depth);

#endif
