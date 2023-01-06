#include <jackal/jackal_compiler.h>
#include <jackal/jackal_string.h>

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
    {
      program->symbols[i] = NULL;
      program->symbols_refs[i] = 0;
    }
}

jkl_word_t jkl_emit_symbol(jkl_program_t *program, jkl_string_t symbol)
{
  jkl_word_t i = program->n_symbols;
  jkl_word_t hash = jkl_string_hash(symbol);
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;

  printf("[jkl_compiler] emitting symbol %s at %d with hash %d\n", symbol, pos, hash);

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
  {
    fprintf(stderr, "Symbol '%s' not found\n", symbol);
    exit(EXIT_FAILURE);
  }

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

jkl_bool_t jkl_set_symbol(jkl_program_t *program, jkl_word_t hash, jkl_heap_object_t *object)
{
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;
  printf("[jkl_compiler] setting symbol %s at %d\n", program->symbols[pos], pos);
  if (pos >= JKL_MAX_CST_VALUES) {
    printf("[jkl_compiler] symbol table overflow\n");
    return 1;
  }

  program->symbols_refs[pos] = object;

  // TODO: check if symbol is already defined
  jkl_heap_object_t *obj = program->symbols_refs[pos];

  printf("[jkl_compiler] symbol %s at %d set\n", program->symbols[pos], pos); 
  printf("[jkl_compiler] symbol %s at %d set to %d\n", program->symbols[pos], pos, obj->value->type);
  printf("[jkl_compiler] symbol %s at %d set to %s\n", program->symbols[pos], pos, obj->value->v_string);
  return 0;
}

jkl_heap_object_t *jkl_get_symbol_ref(jkl_program_t *program, jkl_word_t hash)
{
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;
  if (pos >= JKL_MAX_CST_VALUES) {
    printf("[jkl_compiler] symbol table overflow\n");
    exit(1);
  }

  printf("[jkl_compiler] getting symbol %s at %d\n", program->symbols[pos], pos);
  if (program->symbols[pos] == NULL) {
    printf("[jkl_compiler] symbol %s not found\n", program->symbols[pos]);
    exit(1);
  }
  printf("[jkl_compiler] symbol %s found\n", program->symbols[pos]);

  return &program->symbols_refs[pos];
}

jkl_bool_t jkl_get_2_symbol_ref(jkl_program_t *program, jkl_word_t hash, jkl_heap_object_t *ref)
{
  jkl_word_t pos = hash % JKL_MAX_CST_VALUES;
  if (pos >= JKL_MAX_CST_VALUES) {
    printf("[jkl_compiler] symbol table overflow\n");
    return 1;
  }

  printf("[jkl_compiler] getting symbol %s at %d\n", program->symbols[pos], pos);
  if (program->symbols[pos] == NULL) {
    printf("[jkl_compiler] symbol %s not found\n", program->symbols[pos]);
    exit(1);
  }
  printf("[jkl_compiler] symbol %s found\n", program->symbols[pos]);

  ref = (jkl_heap_object_t*)&program->symbols_refs[pos];
  printf("[jkl_compiler] symbol %s ref %p\n", program->symbols[pos], ref);
  printf("[jkl_compiler] symbol %s ref type %d\n", program->symbols[pos], ref->value->type);
  printf("[jkl_compiler] symbol %s ref value %s\n", program->symbols[pos], ref->value->v_string);
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
        jkl_word_t iconst = jkl_emit_inst_1(program, JKL_CST, ilabel);
        return 0;
      }
    }
  }

  printf("[jkl_compiler]: does not support this operation\n");
  exit(1);

  return 0;
}

jkl_word_t jkl_emit_loop(jkl_program_t *program, jkl_node_t *node)
{
  if (node->type == JKL_NODE_LOOP)
  {
    jkl_word_t i = program->code.n_insts;
    jkl_word_t ilpb = jkl_emit_inst_0(program, JKL_LPB);
    jkl_node_t *block = node->block;
    jkl_word_t iblk = jkl_emit_block(program, block);
    jkl_word_t ilpe = jkl_emit_inst_1(program, JKL_LPE, i);
    return ilpb;
  }

  printf("[jkl_compiler]: does not support this operation\n");
  exit(1);

  return 0;
}

jkl_word_t jkl_emit_block(jkl_program_t *program, jkl_node_t *node)
{
  if (node->type == JKL_NODE_BLOCK)
  {
    printf("[jkl_compiler]: block\n");
    for (jkl_word_t i = 0; i < node->compound.n_nodes; i++)
    {
      printf("[jkl_compiler]: block node %d\n", i);
      jkl_node_t *child = &node->compound.nodes[i];

      if (child->type == JKL_NODE_BINOP)
        {
          printf("[jkl_compiler]: block node %d is binop\n", i);
          jkl_emit_binop(program, child);
        }
      else if (child->type == JKL_NODE_LOOP)
        {
          printf("[jkl_compiler]: block node %d is loop\n", i);
          jkl_emit_loop(program, child);
        }
      else if (child->type == JKL_NODE_BLOCK)
        {
          printf("[jkl_compiler]: block node %d is block\n", i);
          jkl_emit_block(program, child);
        }
      else if (child->type == JKL_NODE_RAISE)
        {
          printf("[jkl_compiler]: emit raise\n");
          jkl_emit_raise(program, child);
        }
      else if (child->type == JKL_NODE_PUTS)
        {
          printf("[jkl_compiler]: emit puts\n");
          jkl_emit_puts(program, child);
        }
      else
        {
          printf("[jkl_compiler]: block node %d is unknown\n", i);
          jkl_print_ast_node(child);
          exit(1);
        }
    }

    return 0;
  }

  printf("[jkl_compiler]: does not support this operation\n");
  exit(1);

  return 0;
}

jkl_word_t jkl_emit_raise(jkl_program_t *program, jkl_node_t *node) {
  if (node->type != JKL_NODE_RAISE) {
    printf("[jkl_compiler]: does not support this operation\n");
    exit(1);
  }

  int imsg = jkl_emit_const_string(program, node->value.s);
  jkl_emit_inst_1(program, JKL_RAI, imsg);
}

jkl_word_t jkl_emit_puts(jkl_program_t *program, jkl_node_t *node) {
  if (node->type != JKL_NODE_PUTS) {
    printf("[jkl_compiler]: does not support this operation\n");
    exit(1);
  }

  jkl_node_t *value = node->value.node;
  if (value->type == JKL_NODE_STRING) {
    int imsg = jkl_emit_const_string(program, value->value.s);
    jkl_emit_inst_1(program, JKL_PTS, imsg);
  } else if (value->type == JKL_NODE_ID) {
    int ilabel = jkl_symbol_load(program, value->value.s);
    int iconst = jkl_emit_inst_1(program, JKL_CST, ilabel);
    jkl_emit_inst_1(program, JKL_PTS, iconst);
  } else {
    printf("[jkl_compiler]: does not support this operation\n");
    exit(1);
  }
}
