#include <jackal.h>

#include "test.h"

static jkl_program_t program;

struct jkl_tbl_entry_t jkl_tbl_value_types[] = {
  [JKL_TYPE_NONE] = "none",
  [JKL_TYPE_BOOL] = "bool",
  [JKL_TYPE_BYTE] = "byte",
  [JKL_TYPE_WORD] = "word",
  [JKL_TYPE_DWORD] = "dword",
  [JKL_TYPE_QWORD] = "qword",
  [JKL_TYPE_QQWORD] = "qqword",
  [JKL_TYPE_FLOAT] = "float",
  [JKL_TYPE_DOUBLE] = "double",
  [JKL_TYPE_LDOUBLE] = "ldouble",
  [JKL_TYPE_CHAR] = "char",
  [JKL_TYPE_STRING] = "string",
  [JKL_TYPE_POINTER] = "pointer",
  [JKL_TYPE_FUNCTION] = "function",
  [JKL_TYPE_REF_OBJECT] = "ref_object",
};

struct jkl_tbl_entry_t jkl_tbl_inst_types [] = {
  [JKL_NOP] = "nop",
  [JKL_CST] = "cst",
  [JKL_PSH] = "psh",
  [JKL_LPB] = "lpb",
  [JKL_LPE] = "lpe",
  [JKL_BRK] = "brk",
  [JKL_RAI] = "rai",
  [JKL_PTS] = "pts",
  [JKL_STR] = "str",
  [JKL_OR] = "or",
  [JKL_AND] = "and",
  [JKL_EQL] = "eql",
  [JKL_NEQ] = "neq",
  [JKL_JCP] = "jcp",
  [JKL_HLT] = "hlt",
};

TEST(compile_empty_block, {
  jkl_program_init(&program);
  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_word_t r = jkl_compile(&program, block);

  ASSERT_EQ(r, 0);
  ASSERT_EQ(program.code.n_insts, 1);
  ASSERT_EQ(program.code.inst[0].type, JKL_HLT);
});

TEST(compile_block_with_let_decl, {
  jkl_program_init(&program);
  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_t *let = jkl_node_new(JKL_NODE_LET);
  let->id = jkl_node_new(JKL_NODE_ID);
  let->assign = jkl_node_new(JKL_NODE_INT);

  let->id->value.s = "x";
  let->assign->value.i = 42;
  jkl_node_append(block, let);

  jkl_word_t r = jkl_compile(&program, block);
  ASSERT_EQ(r, 0);
  ASSERT_EQ(program.code.n_insts, 3);
  ASSERT_EQ(program.code.inst[0].type, JKL_PSH);
  ASSERT_EQ(program.code.inst[1].type, JKL_STR);
  ASSERT_EQ(program.code.inst[2].type, JKL_HLT);
  ASSERT_EQ(program.bss.n_values, 1);
});

int main()
{
  jkl_log("jkl_test", "Jackal automated test runner!");
  jkl_log("jkl_test", "Running tests...");

  RUN_TEST(compile_empty_block);
  RUN_TEST(compile_block_with_let_decl);

  jkl_log("jkl_test", "All tests passed!");
  return 0;
}
