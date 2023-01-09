#include <jackal/jackal_compiler.h>
#include <jackal/jackal_string.h>
#include <jackal/jackal_error.h>

#define JKL_EMIT_CONST(program, P, T, V)               \
  do                                                   \
  {                                                    \
    jkl_word_t i = program->bss.n_values;              \
    program->bss.values[i].type = T;                   \
    program->bss.values[i].v_##P = V;                  \
    program->bss.values[i].mutability = JKL_INMUTABLE; \
    program->bss.n_values++;                           \
    return i;                                          \
  } while (0);

void jkl_program_init(jkl_program_t *program)
{
  program->code.n_insts = 0;
  program->bss.n_values = 0;
  program->n_symbols = 0;

  for (jkl_word_t i = 0; i < JKL_MAX_CST_VALUES; i++)
    program->bss.values[i].type = JKL_TYPE_NONE;

  for (jkl_word_t i = 0; i < JKL_MAX_INST; i++)
    {
      program->code.inst[i].type = JKL_NOP;
      program->code.inst[i].n_args = 0;
    }

  for (jkl_word_t i = 0; i < JKL_MAX_CST_VALUES; i++)
    program->symbols[i] = NULL;
}

jkl_word_t jkl_emit_symbol(jkl_program_t *program, jkl_string_t symbol)
{
  jkl_word_t hash = jkl_string_hash(symbol);
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;

  jkl_log("jkl_compiler", "emitting symbol %s at %d with hash %d", symbol, pos, hash);

  program->symbols[pos] = symbol;
  program->n_symbols++;
  return hash;
}

jkl_word_t jkl_symbol_load(jkl_program_t *program, jkl_string_t symbol)
{
  jkl_word_t hash = jkl_string_hash(symbol);
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;
  jkl_string_t sym = program->symbols[pos];

  if (sym == NULL)
    jkl_error("jkl_compiler", "symbol %s not found", symbol);

  return hash;
}

jkl_bool_t jkl_symbol_exist(jkl_program_t *program, jkl_string_t symbol)
{
  jkl_word_t hash = jkl_string_hash(symbol);
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;
  return program->symbols[pos] != NULL;
}

jkl_bool_t jkl_get_symbol(jkl_program_t *program, jkl_word_t hash, jkl_string_t *symbol)
{
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;
  *symbol = program->symbols[pos];

  if (*symbol == NULL)
    return 1;

  return 0;
}

jkl_word_t jkl_emit_const_bool(jkl_program_t *program, jkl_bool_t value)
{
  JKL_EMIT_CONST(program, bool, JKL_TYPE_BOOL, value);
}

jkl_word_t jkl_emit_const_byte(jkl_program_t *program, jkl_byte_t value)
{
  JKL_EMIT_CONST(program, byte, JKL_TYPE_BYTE, value);
}

jkl_word_t jkl_emit_const_word(jkl_program_t *program, jkl_word_t value)
{
  JKL_EMIT_CONST(program, word, JKL_TYPE_WORD, value);
}

jkl_word_t jkl_emit_const_dword(jkl_program_t *program, jkl_dword_t value)
{
  JKL_EMIT_CONST(program, dword, JKL_TYPE_DWORD, value);
}

jkl_word_t jkl_emit_const_qword(jkl_program_t *program, jkl_qword_t value)
{
  JKL_EMIT_CONST(program, qword, JKL_TYPE_QWORD, value);
}

jkl_word_t jkl_emit_const_float(jkl_program_t *program, jkl_float_t value)
{
  JKL_EMIT_CONST(program, float, JKL_TYPE_FLOAT, value);
}

jkl_word_t jkl_emit_const_double(jkl_program_t *program, jkl_double_t value)
{
  JKL_EMIT_CONST(program, double, JKL_TYPE_DOUBLE, value);
}

jkl_word_t jkl_emit_const_ldouble(jkl_program_t *program, jkl_ldouble_t value)
{
  JKL_EMIT_CONST(program, ldouble, JKL_TYPE_LDOUBLE, value);
}

jkl_word_t jkl_emit_const_char(jkl_program_t *program, jkl_char_t value)
{
  JKL_EMIT_CONST(program, char, JKL_TYPE_CHAR, value);
}

jkl_word_t jkl_emit_const_qqword(jkl_program_t *program, jkl_qqword_t value)
{
  JKL_EMIT_CONST(program, qqword, JKL_TYPE_QQWORD, value);
}

jkl_word_t jkl_emit_const_string(jkl_program_t *program, jkl_string_t value)
{
  JKL_EMIT_CONST(program, string, JKL_TYPE_STRING, value);
}

jkl_word_t jkl_emit_const_pointer(jkl_program_t *program, jkl_pointer_t value)
{
  JKL_EMIT_CONST(program, pointer, JKL_TYPE_POINTER, value);
}

jkl_word_t jkl_emit_const_function(jkl_program_t *program, jkl_function_t value)
{
  JKL_EMIT_CONST(program, function, JKL_TYPE_FUNCTION, value);
}

jkl_word_t jkl_emit_inst_0(jkl_program_t *program, jkl_inst_type_t type)
{
  jkl_word_t i = program->code.n_insts;
  program->code.inst[i].type = type;
  program->code.inst[i].n_args = 1;
  program->code.n_insts++;
  return i;
}


jkl_word_t jkl_emit_inst_1(jkl_program_t *program, jkl_inst_type_t type, jkl_word_t arg)
{
  jkl_word_t i = program->code.n_insts;
  program->code.inst[i].type = type;
  program->code.inst[i].n_args = 1;
  program->code.inst[i].args[0] = arg;
  program->code.n_insts++;
  return i;
}

jkl_word_t jkl_emit_inst_2(jkl_program_t *program, jkl_inst_type_t type, jkl_word_t lhs, jkl_word_t rhs)
{
  jkl_word_t i = program->code.n_insts;
  program->code.inst[i].type = type;
  program->code.inst[i].n_args = 2;
  program->code.inst[i].args[0] = lhs;
  program->code.inst[i].args[1] = rhs;
  program->code.n_insts++;
  return i;
}

jkl_word_t jkl_emit_binop(jkl_program_t *program, jkl_node_t *node)
{
  if (node->type == JKL_NODE_BINOP)
  {
    if (node->binop.op == JKL_OP_ASSIGN)
    {
      jkl_word_t ilhs = jkl_emit_symbol(program, node->binop.left->value.s);
      jkl_node_t *rhs = node->binop.right;

      if (rhs->type == JKL_NODE_STRING)
      {
        jkl_word_t irhs = jkl_emit_const_string(program, rhs->value.s);
        jkl_word_t iop = jkl_emit_inst_2(program, JKL_STR, ilhs, irhs);
        return iop;
      }

      if (rhs->type == JKL_NODE_INT)
      {
        jkl_word_t irhs = jkl_emit_const_word(program, rhs->value.i);
        jkl_word_t iop = jkl_emit_inst_2(program, JKL_PSH, ilhs, irhs);
        return iop;
      }

      if (rhs->type == JKL_NODE_FLOAT)
      {
        jkl_word_t irhs = jkl_emit_const_double(program, rhs->value.f);
        jkl_word_t iop = jkl_emit_inst_2(program, JKL_PSH, ilhs, irhs);
        return iop;
      }

      if (rhs->type == JKL_NODE_ID)
      {
        jkl_word_t ilabel = jkl_symbol_load(program, rhs->value.s);
        jkl_emit_inst_1(program, JKL_CST, ilabel);
        return 0;
      }
    }
  }

  jkl_error("jkl_compiler", "does not support this operation");
}

jkl_word_t jkl_emit_loop(jkl_program_t *program, jkl_node_t *node)
{
  if (node->type == JKL_NODE_LOOP)
  {
    jkl_word_t i = program->code.n_insts;
    jkl_word_t ilpb = jkl_emit_inst_0(program, JKL_LPB);
    jkl_node_t *block = node->block;
    jkl_emit_block(program, block);
    jkl_emit_inst_1(program, JKL_LPE, i);
    return ilpb;
  }

  jkl_error("jkl_compiler", "does not support this operation");
}

jkl_word_t jkl_emit_block(jkl_program_t *program, jkl_node_t *node)
{
  if (node->type == JKL_NODE_BLOCK)
  {
    jkl_log("jkl_compiler", "block");
    for (jkl_word_t i = 0; i < node->compound.n_nodes; i++)
    {
      jkl_log("jkl_compiler", "block node %d", i);
      jkl_node_t *child = node->compound.nodes[i];

      if (child->type == JKL_NODE_BINOP)
        {
          jkl_log("jkl_compiler", "block node %d is binop", i);
          jkl_emit_binop(program, child);
        }
      else if (child->type == JKL_NODE_LOOP)
        {
          jkl_log("jkl_compiler", "block node %d is loop", i);
          jkl_emit_loop(program, child);
        }
      else if (child->type == JKL_NODE_BLOCK)
        {
          jkl_log("jkl_compiler", "block node %d is block", i);
          jkl_emit_block(program, child);
        }
      else if (child->type == JKL_NODE_RAISE)
        {
          jkl_log("jkl_compiler", "emit raise");
          jkl_emit_raise(program, child);
        }
      else if (child->type == JKL_NODE_PUTS)
        {
          jkl_log("jkl_compiler", "emit puts");
          jkl_emit_puts(program, child);
        }
      else
          jkl_error("jkl_compiler", "block node %d is unknown", i);
    }

    return 0;
  }

  jkl_error("jkl_compiler", "does not support this operation");
}

jkl_word_t jkl_emit_raise(jkl_program_t *program, jkl_node_t *node) {
  if (node->type != JKL_NODE_RAISE)
    jkl_error("jkl_compiler", "does not support this operation");

  int imsg = jkl_emit_const_string(program, node->value.s);
  jkl_emit_inst_1(program, JKL_RAI, imsg);
  return 0;
}

jkl_word_t jkl_emit_puts(jkl_program_t *program, jkl_node_t *node) {
  if (node->type != JKL_NODE_PUTS)
    jkl_error("jkl_compiler", "does not support this operation");

  jkl_node_t *value = node->node;
  jkl_log("jkl_compiler", "puts value type 0x%04x", value->type);

  if (value->type == JKL_NODE_STRING) {
    int imsg = jkl_emit_const_string(program, value->value.s);
    jkl_emit_inst_2(program, JKL_PSH, JKL_TYPE_STRING, imsg);
    jkl_emit_inst_1(program, JKL_PTS, imsg);
  } else if (value->type == JKL_NODE_ID) {
    int ilabel = jkl_symbol_load(program, value->value.s);
    int iconst = jkl_emit_inst_1(program, JKL_CST, ilabel);
    jkl_emit_inst_1(program, JKL_PTS, iconst);
  } else
    jkl_error("jkl_compiler", "does not support this operation");

  return 0;
}

jkl_word_t jkl_emit_class(jkl_program_t *program, jkl_class_t *klass)
{
  jkl_error("jkl_compiler", "not implemented yet");
}

jkl_word_t jkl_compile(jkl_program_t *program, jkl_node_t *node)
{
  if (node->type == JKL_NODE_BLOCK)
    return jkl_emit_block(program, node);

  jkl_error("jkl_compiler", "does not support this operation")
}
