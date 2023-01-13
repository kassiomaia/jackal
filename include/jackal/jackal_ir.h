#ifndef JACKAL_IR_H
#define JACKAL_IR_H

/*
 * Jackal Intermediate Representation
 */

typedef enum {
  JKL_IR_NOPE     = 0x00ca,         // [0x00ca]: 0000 0000 1100 1010

  /* MEMORY */
  JKL_IR_ALLOC    = 0x01cb,         // [0x01cb]: 0000 0001 1100 1011

  JKL_IR_LOAD     = 0x01cc,         // [0x01cc]: 0000 0001 1100 1100
  JKL_IR_STORE    = 0x01cd,         // [0x01cd]: 0000 0001 1100 1101

  /* STACK */
  JKL_IR_PUSHI    = 0x01ce,         // [0x01ce]: 0000 0001 1100 1110
  JKL_IR_PUSHF    = 0x01cf,         // [0x01cf]: 0000 0001 1100 1111
  JKL_IR_PUSHS    = 0x01d0,
  JKL_IR_POPI     = 0x01d1,         // [0x01d1]: 0000 0001 1101 0001
  JKL_IR_POPF     = 0x01d2,         // [0x01d2]: 0000 0001 1101 0010
  JKL_IR_POPS     = 0x01d3,         // [0x01d3]: 0000 0001 1101 0011

  /* OPERATIONS */
  JKL_IR_ADD      = 0x00d4,         // [0x00d4]: 0000 0000 1101 0100
  JKL_IR_SUB      = 0x00d5,         // [0x00d5]: 0000 0000 1101 0101
  JKL_IR_MUL      = 0x00d6,         // [0x00d6]: 0000 0000 1101 0110
  JKL_IR_DIV      = 0x00d7,         // [0x00d7]: 0000 0000 1101 0111
  JKL_IR_MOD      = 0x00d8,         // [0x00d8]: 0000 0000 1101 1000

  /* LOGIC */
  JKL_IR_EQL      = 0x00d9,         // [0x00d9]: 0000 0000 1101 1001
  JKL_IR_NEQ      = 0x00da,         // [0x00da]: 0000 0000 1101 1010
  JKL_IR_GT       = 0x00db,         // [0x00db]: 0000 0000 1101 1011
  JKL_IR_GTE      = 0x00dc,         // [0x00dc]: 0000 0000 1101 1100
  JKL_IR_LT       = 0x00dd,         // [0x00dd]: 0000 0000 1101 1101
  JKL_IR_LTE      = 0x00de,         // [0x00de]: 0000 0000 1101 1110
  JKL_IR_AND      = 0x00df,         // [0x00df]: 0000 0000 1101 1111
  JKL_IR_OR       = 0x00e0,         // [0x00e0]: 0000 0000 1110 0000
  JKL_IR_NOT      = 0x00e1,         // [0x00e1]: 0000 0000 1110 0001

  /* FLOW CONTROL */
  JKL_IR_JMP      = 0x01e2,         // [0x01e2]: 0000 0001 1110 0010
  JKL_IR_JCP      = 0x01e3,         // [0x01e3]: 0000 0001 1110 0011
  JKL_IR_CALL     = 0x01e4,         // [0x01e4]: 0000 0001 1110 0100
  JKL_IR_RET      = 0x00e5,         // [0x00e5]: 0000 0000 1110 0101
  JKL_IR_HALT     = 0x00e6,         // [0x00e6]: 0000 0000 1110 0110
} jkl_ir_type_t;

#define JKL_IR_TYPE_SIZE sizeof(jkl_ir_type_t)
#define JKL_IR_ARG_SIZE sizeof(jkl_qqword_t)
#define JKL_IR_ARG_COUNT 3

typedef struct {
  jkl_ir_type_t type;
  jkl_qqword_t args[JKL_IR_ARG_COUNT];
} jkl_ir_t;

#define JKL_IR_SIZE sizeof(jkl_ir_t)
#define JKL_EMIT_IR(type, ...) \
  (jkl_ir_t){type, {__VA_ARGS__}}

#define JKL_EMIT_IR_NOPE() \
  (jkl_ir_t){JKL_IR_NOPE, {0, 0, 0}}

#define JKL_ARITY(type) (type & 0x00ff)
#define JKL_IS_ARITY(type, arity) (JKL_ARITY(type) == arity)
#define JKL_IS_ARITY_0(type) JKL_IS_ARITY(type, 0)
#define JKL_IS_ARITY_1(type) JKL_IS_ARITY(type, 1)
#define JKL_IS_ARITY_2(type) JKL_IS_ARITY(type, 2)

typedef struct {
  jkl_ir_t *ir;
  jkl_qqword_t n_irs;
  jkl_byte_t bss[(1 << (sizeof(jkl_word_t) * 8)) - 1];
} jkl_ir_code_t;

#define JKL_IR_CODE_INIT {NULL, 0}
#define JKL_IR_CODE_ALLOC(size) \
  (jkl_ir_code_t){malloc(size * JKL_IR_SIZE), size}

#define JKL_IR_CODE_FREE(code) \
  free(code.ir)

#define JKL_IR_CODE_PUSH(code, _ir) \
  code.ir[code.n_irs++] = _ir; \
  code.ir = realloc(code.ir, code.n_irs * JKL_IR_SIZE); \
  if (code.ir == NULL) \
    jkl_error("jkl_ir", "ERROR: Could not reallocate IR code\n"); \
  code.ir[code.n_irs] = JKL_EMIT_IR_NOPE();

#define JKL_IR_CODE_STORE(code, filename) \
  FILE *out = fopen(filename, "wb"); \
  fwrite(code.ir, JKL_IR_SIZE, code.n_irs, out); \
  fclose(out);

void jkl_ir_code_init(jkl_ir_code_t *ir_code, jkl_word_t size);
jkl_word_t jkl_ir_code_push(jkl_ir_code_t *ir_code, jkl_ir_t ir);
void jkl_ir_code_free(jkl_ir_code_t *ir_code);
jkl_word_t jkl_ir_code_save(jkl_ir_code_t *ir_code, const char* filename);
void jkl_ir_store_string(jkl_ir_code_t *ir_code, jkl_string_t *string);

#endif

