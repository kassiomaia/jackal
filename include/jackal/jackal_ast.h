#ifndef JACKAL_AST_H
#define JACKAL_AST_H

#include <stdlib.h>
#include <stdio.h>
#include <jackal/jackal_types.h>

/*
 * AST
 */

jkl_word_t jkl_print_ast_node(jkl_node_t *node, jkl_word_t depth);

#define JKL_AST_BINOP(L, O, R) \
  (jkl_node_t){                \
      .type = JKL_NODE_BINOP,  \
      .binop = {               \
          .left = L,           \
          .op = O,             \
          .right = R,          \
      }};

#define JKL_AST_ID(S)                       \
  (jkl_node_t)                              \
  {                                         \
    .type = JKL_NODE_ID, .value = {.s = S } \
  }
#define JKL_AST_INT(I)                       \
  (jkl_node_t)                               \
  {                                          \
    .type = JKL_NODE_INT, .value = {.i = I } \
  }
#define JKL_AST_STRING(S)                       \
  (jkl_node_t)                                  \
  {                                             \
    .type = JKL_NODE_STRING, .value = {.s = S } \
  }
#define JKL_AST_FLOAT(F)                       \
  (jkl_node_t)                                 \
  {                                            \
    .type = JKL_NODE_FLOAT, .value = {.f = F } \
  }
#define JKL_AST_BLOCK()     \
  (jkl_node_t)              \
  {                         \
    .type = JKL_NODE_BLOCK, \
  }
#define JKL_AST_LOOP(B)    \
  (jkl_node_t)             \
  {                        \
    .type = JKL_NODE_LOOP, \
    .block = B,            \
  }
#define JKL_AST_RAISE(S)      \
  (jkl_node_t)                \
  {                           \
    .type = JKL_NODE_RAISE,   \
    .value = { .s = S },      \
  }
#define JKL_AST_PUTS(S)      \
  (jkl_node_t)               \
  {                          \
    .type = JKL_NODE_PUTS,   \
    .value = { .node = S },  \
  }

jkl_word_t jkl_node_append(jkl_node_t *node, jkl_node_t *child);

#endif
