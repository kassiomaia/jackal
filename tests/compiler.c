#include <jackal.h>
#include <check.h>

START_TEST(test_jkl_program_new)
{
  jkl_program_t *program = jkl_program_new();
  ck_assert_ptr_null(program->ast_prog_root);
  ck_assert_int_eq(program->n_errors, 0);
  ck_assert_int_eq(program->n_warnings, 0);
  ck_assert_int_eq(program->n_notes, 0);
  ck_assert_int_eq(program->n_ids, 0);
  ck_assert_int_eq(program->n_funcs, 0);
}
END_TEST

START_TEST(test_jkl_compile)
{
  jkl_program_t *program = jkl_program_new();
  program->ast_prog_root = jkl_node_new(JKL_NODE_BLOCK);

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);
  ck_assert_int_eq(program->n_errors, 0);
  ck_assert_int_eq(program->n_warnings, 0);
  ck_assert_int_eq(program->n_notes, 0);
  ck_assert_int_eq(program->n_ids, 0);
  ck_assert_int_eq(program->n_funcs, 0);
}
END_TEST

START_TEST(test_jkl_compile_check_ir)
{
  jkl_program_t *program = jkl_program_new();
  program->ast_prog_root = jkl_node_new(JKL_NODE_BLOCK);

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);
  ck_assert_int_eq(program->ir_code->n_irs, 1);

  jkl_ir_type_t sequence[] = {
    JKL_IR_HALT
  };

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, sequence[i]);
  }
}
END_TEST


#define DEFINE_AST_HELPER(T_, K_, Z_)\
jkl_node_t* jkl_decl_let_with_##T_(char *name, jkl_##T_##_t value) \
{ \
  jkl_node_t *let = jkl_node_new(JKL_NODE_LET); \
  let->id = jkl_node_new(JKL_NODE_ID); \
  let->expr = jkl_node_new(JKL_NODE_##Z_); \
  let->id->value.s = name; \
  let->expr->value.K_ = value; \
  return let; \
}

DEFINE_AST_HELPER(qword, i, INT)
DEFINE_AST_HELPER(string, s, STRING)

START_TEST(test_jkl_compile_check_let_with_int)
{
  jkl_program_t *program = jkl_program_new();

  // let x = 42
  jkl_node_t *let = jkl_decl_let_with_qword("x", 42);
  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, let);

  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t sequence[] = {
    JKL_IR_ALLOC,
    JKL_IR_PUSHI,
    JKL_IR_STORE,
    JKL_IR_HALT
  };

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, sequence[i]);
  }
}
END_TEST

START_TEST(test_jkl_compile_check_let_with_string)
{
  jkl_program_t *program = jkl_program_new();

  // let x = 42
  jkl_node_t *let = jkl_decl_let_with_string("x", "value");
  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, let);

  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t sequence[] = {
    JKL_IR_ALLOC,
    JKL_IR_LOAD,
    JKL_IR_STORE,
    JKL_IR_HALT
  };

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, sequence[i]);
  }
}
END_TEST

Suite *jkl_compiler_suite()
{
  Suite *s = suite_create("jkl_compiler");

  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_jkl_program_new);
  tcase_add_test(tc_core, test_jkl_compile);
  tcase_add_test(tc_core, test_jkl_compile_check_ir);
  tcase_add_test(tc_core, test_jkl_compile_check_let_with_int);
  tcase_add_test(tc_core, test_jkl_compile_check_let_with_string);
  suite_add_tcase(s, tc_core);

  return s;
}
