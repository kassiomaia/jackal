#ifndef JACKAL_TYPES_H
#define JACKAL_TYPES_H

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

typedef struct
{
  enum
  {
    JKL_TYPE_NONE,
    JKL_TYPE_BOOL,
    JKL_TYPE_BYTE,
    JKL_TYPE_WORD,
    JKL_TYPE_DWORD,
    JKL_TYPE_QWORD,
    JKL_TYPE_QQWORD,
    JKL_TYPE_FLOAT,
    JKL_TYPE_DOUBLE,
    JKL_TYPE_LDOUBLE,
    JKL_TYPE_CHAR,
    JKL_TYPE_STRING,
    JKL_TYPE_POINTER,
    JKL_TYPE_FUNCTION,
    JKL_TYPE_REF_OBJECT,
  } type;
  enum
  {
    JKL_INMUTABLE,
    JKL_MUTABLE,
  } mutability;
  union
  {
    jkl_bool_t v_bool;
    jkl_byte_t v_byte;
    jkl_word_t v_word;
    jkl_dword_t v_dword;
    jkl_qword_t v_qword;
    jkl_qqword_t v_qqword;
    jkl_float_t v_float;
    jkl_double_t v_double;
    jkl_ldouble_t v_ldouble;
    jkl_char_t v_char;
    jkl_string_t v_string;
    jkl_pointer_t v_pointer;
    jkl_function_t v_function;
  };
} jkl_value_t;

/*
 * Error
 */

typedef unsigned int jkl_error_t;
typedef jkl_string_t jkl_error_name_t;

/*
 * AST
 */

typedef struct jkl_node jkl_node_t;

struct jkl_node
{
  enum
  {
    JKL_NODE_INT,
    JKL_NODE_FLOAT,
    JKL_NODE_STRING,
    JKL_NODE_ID,
    JKL_NODE_BINOP,
    JKL_NODE_LOOP,
    JKL_NODE_BLOCK,
    JKL_NODE_RAISE,
    JKL_NODE_PUTS,
  } type;
  union
  {
    jkl_qqword_t i;
    jkl_string_t s;
    jkl_float_t f;
    jkl_node_t *node;
  } value;
  struct
  {
    jkl_node_t *left;
    enum
    {
      JKL_OP_ASSIGN,
    } op;
    jkl_node_t *right;
  } binop;
  struct {
    jkl_node_t *nodes;
    jkl_word_t n_nodes;
  } compound;
  jkl_node_t *block;
};

/*
 * Instruction set architecture
 */

typedef enum
{
  JKL_NOP = 0x00ff,
  JKL_CST = 0x00fe,
  JKL_PSH = 0x00fd,
  JKL_LPB = 0x00fc,
  JKL_LPE = 0x00fb,
  JKL_BRK = 0x00fa,
  JKL_RAI = 0x00f9,
  JKL_PTS = 0x00f8,
  JKL_STR = 0x00f7,
} jkl_inst_type_t;

#define JKL_MAX_INST_ARGS 8

typedef struct
{
  jkl_inst_type_t type;
  jkl_word_t args[JKL_MAX_INST_ARGS];
  jkl_word_t n_args;
} jkl_inst_t;

#define JKL_MAX_CST_VALUES 256
#define JKL_MAX_INST 1024

typedef struct
{
  struct
  {
    jkl_inst_t inst[JKL_MAX_INST];
    jkl_word_t n_insts;
  } code;
  struct
  {
    jkl_value_t values[JKL_MAX_CST_VALUES];
    jkl_qqword_t n_values;
  } bss;
  jkl_string_t symbols[JKL_MAX_CST_VALUES];
  jkl_pointer_t symbols_refs[JKL_MAX_CST_VALUES];
  jkl_qqword_t n_symbols;
} jkl_program_t;

/*
 * VM
 */

#define JKL_VM_STACK_FRAME_SIZE 1024
#define JKL_VM_STACK_SIZE 1024

typedef struct
{
  jkl_qqword_t pc;
  jkl_qqword_t bp;
  jkl_qqword_t sp;
  jkl_qqword_t fp;
  jkl_qqword_t size;
  jkl_value_t values[JKL_VM_STACK_FRAME_SIZE];
} jkl_stack_frame_t;

typedef struct
{
  struct
  {
    jkl_pointer_t refs;
    jkl_word_t n_refs;
  } refs;
  struct
  {
    jkl_qqword_t size;
    jkl_pointer_t *data;
  } data;
  struct
  {
    jkl_bool_t is_tagged;
    jkl_word_t created_at;
    jkl_word_t updated_at;
  } opts;
  jkl_value_t *value;
} jkl_heap_object_t;

typedef struct
{
  jkl_heap_object_t *root;
  jkl_qqword_t n_objects;
} jkl_heap_t;

typedef struct
{
  jkl_qqword_t pc;
  jkl_program_t *program;
  jkl_stack_frame_t *frame;
  jkl_heap_t *heap;
  jkl_bool_t halted;
  struct {
    FILE *in;
    FILE *out;
    FILE *err;
  } io;
} jkl_vm_t;

#endif
