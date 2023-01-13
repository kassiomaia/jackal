#ifndef JACKAL_TYPES_H
#define JACKAL_TYPES_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Basic types
 */

typedef char jkl_byte_t;
typedef short jkl_word_t;
typedef int jkl_dword_t;
typedef long jkl_qword_t;
typedef long long jkl_qqword_t;
typedef float jkl_float_t;
typedef double jkl_double_t;
typedef long double jkl_ldouble_t;
typedef char *jkl_string_t;
typedef void *jkl_pointer_t;
typedef void (*jkl_function_t)(void);
typedef char jkl_bool_t;
typedef char jkl_char_t;
typedef void jkl_void_t;
typedef void *jkl_any_t;

/*
 * Error
 */

typedef unsigned int jkl_error_t;
typedef jkl_string_t jkl_error_name_t;

/*
 * AST
 */

typedef struct jkl_node jkl_node_t;
typedef enum {
  JKL_NODE_NONE,
  JKL_NODE_INT,
  JKL_NODE_FLOAT,
  JKL_NODE_STRING,
  JKL_NODE_ID,
  JKL_NODE_LET,
  JKL_NODE_LOOP,
  JKL_NODE_BLOCK,
  JKL_NODE_RAISE,
  JKL_NODE_IF,
  JKL_NODE_ASSIGNMENT,
  JKL_NODE_CALL,
  JKL_NODE_RETURN,
  JKL_NODE_BINOP,
  JKL_NODE_FUNC,
  JKL_NODE_PARAM,
  JKL_NODE_PARAMS,
} jkl_node_type_t;

typedef enum {
  JKL_OP_NONE = 0,
  JKL_OP_EQL,
  JKL_OP_NEQ,
  JKL_OP_GT,
  JKL_OP_LT,
  JKL_OP_GTE,
  JKL_OP_LTE,
  JKL_OP_PLUS,
  JKL_OP_MINUS,
  JKL_OP_MUL,
  JKL_OP_DIV,
  JKL_OP_MOD,
  JKL_OP_AND,
  JKL_OP_OR,
  JKL_OP_NOT,
} jkl_op_t;

struct jkl_node {
  jkl_node_type_t type;
  union {
    jkl_qqword_t i;
    jkl_string_t s;
    jkl_float_t f;
  } value;
  struct {
    jkl_node_t *left;
    jkl_op_t op;
    jkl_node_t *right;
  } binop;
  struct {
    jkl_node_t **nodes;
    jkl_word_t n_nodes;
  } compound;
  jkl_node_t *expr;
  jkl_node_t *block;
  jkl_node_t *node;
  jkl_node_t *parent;
  jkl_node_t *id;
  jkl_node_t *assign;
  jkl_node_t *params;
};

#endif
