#include <jackal.h>

void jkl_print_dash_line()
{
  jkl_word_t n = 121;
  for (jkl_word_t i = 0; i < n; i++)
    printf("-");

  printf("\n");
}

void jkl_symbol_table_dump(jkl_program_t *program)
{
  jkl_print_dash_line();
  printf("Symbol Table (%lld)\n", program->n_symbols);
  jkl_print_dash_line();

  for (jkl_word_t i = 0; i < JKL_MAX_CST_VALUES; i++)
  {
    jkl_string_t sym = program->symbols[i];
    if (sym == NULL)
      continue;

    printf("%d: %s\n", i, sym);
  }
  jkl_print_dash_line();
}

void jkl_bss_dump(jkl_program_t *program)
{
  printf("BSS (%lld)\n", program->bss.n_values);
  jkl_print_dash_line();

  for (jkl_qqword_t i = 0; i < program->bss.n_values; i++)
  {
    switch (program->bss.values[i].type)
    {
    case JKL_TYPE_BOOL:
      printf("%lld: %s\n", i, program->bss.values[i].v_bool ? "true" : "false");
      break;
    case JKL_TYPE_BYTE:
      printf("%lld: %d\n", i, program->bss.values[i].v_byte);
      break;
    case JKL_TYPE_WORD:
      printf("%lld: %d\n", i, program->bss.values[i].v_word);
      break;
    case JKL_TYPE_DWORD:
      printf("%lld: %d\n", i, program->bss.values[i].v_dword);
      break;
    case JKL_TYPE_QWORD:
      printf("%lld: %ld\n", i, program->bss.values[i].v_qword);
      break;
    case JKL_TYPE_QQWORD:
      printf("%lld: %lld\n", i, program->bss.values[i].v_qqword);
      break;
    case JKL_TYPE_FLOAT:
      printf("%lld: %f\n", i, program->bss.values[i].v_float);
      break;
    case JKL_TYPE_DOUBLE:
      printf("%lld: %f\n", i, program->bss.values[i].v_double);
      break;
    case JKL_TYPE_LDOUBLE:
      printf("%lld: %Lf\n", i, program->bss.values[i].v_ldouble);
      break;
    case JKL_TYPE_CHAR:
      printf("%lld: %c\n", i, program->bss.values[i].v_char);
      break;
    case JKL_TYPE_STRING:
      printf("%lld: %s\n", i, program->bss.values[i].v_string);
      break;
    case JKL_TYPE_POINTER:
      printf("%lld: %p\n", i, program->bss.values[i].v_pointer);
      break;
    case JKL_TYPE_FUNCTION:
      printf("%lld: %p\n", i, program->bss.values[i].v_function);
      break;
    default:
      printf("Unknown type: %d\n", program->bss.values[i].type);
      break;
    }
  }
  jkl_print_dash_line();
}

jkl_string_t jkl_inst_name(jkl_inst_t *inst)
{
  jkl_string_t name;
  switch (inst->type)
  {
  case JKL_NOP:
    return "NOP";
  case JKL_CST:
    return "CST";
  case JKL_PSH:
    return "PSH";
  case JKL_PTS:
    return "PTS";
  case JKL_RAI:
    return "RAI";
  case JKL_LPB:
    return "LPB";
  case JKL_LPE:
    return "LPE";
  case JKL_STR:
    return "STR";
  default:
    return "Unknown";
  }
}

void jkl_decoded_code_dump(jkl_program_t *program) {
  printf("TEXT\n");
  jkl_print_dash_line();

  for (jkl_word_t i = 0; i < program->code.n_insts; i++) {
    jkl_inst_t *inst = &program->code.inst[i];
    printf("%d: %s", i, jkl_inst_name(inst));

    switch (inst->type) {
    case JKL_NOP:
      break;
    case JKL_CST:
      printf("  0x%04x\n", inst->args[0]);
      break;
    case JKL_PSH:
      printf("  0x%04x0x%04x\n", inst->args[0], inst->args[1]);
      break;
    case JKL_PTS:
      printf("  0x%04x\n", inst->args[0]);
      break;
    case JKL_RAI:
      printf("  0x%04x\n", inst->args[0]);
      break;
    case JKL_LPB:
      printf("  0x%04x\n", inst->args[0]);
      break;
    case JKL_LPE:
      printf("  0x%04x\n", inst->args[0]);
      break;
    case JKL_STR:
      printf("  0x%04x\n", inst->args[0]);
      break;
    default:
      break;
    }
  }
}

void jkl_code_dump(jkl_program_t *program)
{
  jkl_word_t cols = 0;
  jkl_word_t rows = 0;

  printf("TEXT\n");
  jkl_print_dash_line();

  printf("%08x: ", rows);
  for (jkl_word_t i = 0; i < JKL_MAX_INST; i++)
  {
    jkl_inst_t *inst = &program->code.inst[i];
    printf("0x%04x ", inst->type);
    if (inst->type == JKL_CST)
    {
      cols++;
      printf("0x%04x ", inst->args[0]);
    }
    else if (inst->type == JKL_STR)
    {
      cols += 2;
      printf("0x%04x 0x%04x ", inst->args[0], inst->args[1]);
    }

    cols++;

    if (cols == 16)
    {
      printf("\n");
      cols = 0;
      rows++;
      printf("%.8d: ", rows);
    }
  }
  printf("\n");
  jkl_print_dash_line();
}

void jkl_dump(jkl_program_t *program)
{
  jkl_print_dash_line();
  printf("Program Dump\n");

  jkl_symbol_table_dump(program);
  jkl_bss_dump(program);
  jkl_code_dump(program);
  jkl_decoded_code_dump(program);
}

void jkl_dump_value(jkl_value_t *value) {
  printf("[jkl_dump] jkl_value_t: ");
  if (value == NULL) {
    printf("NULL\n");
    return;
  }

  switch (value->type) {
    case JKL_TYPE_NONE:
      printf("JKL_TYPE_NONE");
      break;
    case JKL_TYPE_BOOL:
      printf("JKL_TYPE_BOOL: %s", value->v_bool ? "true" : "false");
      break;
    case JKL_TYPE_BYTE:
      printf("JKL_TYPE_BYTE: %d", value->v_byte);
      break;
    case JKL_TYPE_WORD:
      printf("JKL_TYPE_WORD: %d", value->v_word);
      break;
    case JKL_TYPE_DWORD:
      printf("JKL_TYPE_DWORD: %d", value->v_dword);
      break;
    case JKL_TYPE_QWORD:
      printf("JKL_TYPE_QWORD: %ld", value->v_qword);
      break;
    case JKL_TYPE_QQWORD:
      printf("JKL_TYPE_QQWORD: %lld", value->v_qqword);
      break;
    case JKL_TYPE_FLOAT:
      printf("JKL_TYPE_FLOAT: %f", value->v_float);
      break;
    case JKL_TYPE_DOUBLE:
      printf("JKL_TYPE_DOUBLE: %f", value->v_double);
      break;
    case JKL_TYPE_LDOUBLE:
      printf("JKL_TYPE_LDOUBLE: %Lf", value->v_ldouble);
      break;
    case JKL_TYPE_CHAR:
      printf("JKL_TYPE_CHAR: %c", value->v_char);
      break;
    case JKL_TYPE_STRING:
      printf("JKL_TYPE_STRING: %s", value->v_string);
      break;
    case JKL_TYPE_POINTER:
      printf("JKL_TYPE_POINTER: %p", value->v_pointer);
      break;
    case JKL_TYPE_FUNCTION:
      printf("JKL_TYPE_FUNCTION: %p", value->v_function);
      break;
    default:
      printf("Unknown type: %d", value->type);
      break;
  }
  printf("\n");
}
