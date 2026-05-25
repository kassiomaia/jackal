#include <jackal.h>
#ifdef JKL_NO_CHECK
#  include "no_check.h"
#else
#  include <check.h>
#endif

/*
 * AST Helpers
 */

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

#define DECL_AST_TYPE(T_) (jkl_node_new(JKL_NODE_##T_))

DEFINE_AST_HELPER(qword, i, INT)
DEFINE_AST_HELPER(string, s, STRING)

static jkl_node_t *make_int_node(jkl_qqword_t v)
{
  jkl_node_t *n = jkl_node_new(JKL_NODE_INT);
  n->value.i = v;
  return n;
}

static jkl_node_t *make_block1(jkl_node_t *stmt)
{
  jkl_node_t *b = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(b, stmt);
  return b;
}

/*
 * Compiler tests
 */

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

/*
 * Empty program
 * IR:
 *  HALT
 */

START_TEST(test_jkl_compile_check_ir)
{
  jkl_program_t *program = jkl_program_new();
  program->ast_prog_root = jkl_node_new(JKL_NODE_BLOCK);

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);
  ck_assert_int_eq(program->ir_code->n_irs, 1);

  jkl_ir_type_t output[] = {
    JKL_IR_HALT
  };

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }
}
END_TEST


/*
 * let x = 42
 * IR:
 *  ALLOC
 *  PUSHI
 *  STORE
 *  HALT
 */

START_TEST(test_jkl_compile_check_let_with_int)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *let = jkl_decl_let_with_qword("x", 42);
  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, let);

  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t output[] = {
    JKL_IR_ALLOC,
    JKL_IR_PUSHI,
    JKL_IR_STORE,
    JKL_IR_HALT
  };

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }
}
END_TEST

/*
 * let x = "value"
 * IR:
 *  ALLOC
 *  LOAD
 *  STORE
 *  HALT
 */

START_TEST(test_jkl_compile_check_let_with_string)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *let = jkl_decl_let_with_string("x", "value");
  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, let);

  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t output[] = {
    JKL_IR_ALLOC,
    JKL_IR_LOAD,
    JKL_IR_STORE,
    JKL_IR_HALT
  };

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }
}
END_TEST

START_TEST(test_jkl_compile_check_with_loop)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *let = jkl_decl_let_with_string("x", "value");
  jkl_node_t *loop = jkl_node_new(JKL_NODE_LOOP);
  jkl_node_t *loop_block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);

  jkl_node_append(loop_block, let);
  loop->block = loop_block;
  jkl_node_append(block, loop);

  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t output[] = {
    JKL_IR_ALLOC,
    JKL_IR_LOAD,
    JKL_IR_STORE,
    JKL_IR_JMP,
    JKL_IR_HALT
  };

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }

  /* the loop back-edge jumps to the first body instruction (index 0) */
  ck_assert_int_eq(program->ir_code->ir[3].args[0], 0);
}
END_TEST

/*
 * if cond { let x := 1 }
 * IR (indices):
 *  0 PUSHI  (cond literal 1)
 *  1 JCP -> 5    (skip the then-block when false)
 *  2 ALLOC
 *  3 PUSHI
 *  4 STORE
 *  5 HALT
 */

START_TEST(test_jkl_compile_if_without_else)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *iff = jkl_node_new(JKL_NODE_IF);
  iff->expr = make_int_node(1);
  iff->block = make_block1(jkl_decl_let_with_qword("x", 1));

  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, iff);
  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t output[] = {
    JKL_IR_PUSHI,
    JKL_IR_JCP,
    JKL_IR_ALLOC,
    JKL_IR_PUSHI,
    JKL_IR_STORE,
    JKL_IR_HALT
  };

  ck_assert_int_eq(program->ir_code->n_irs, 6);
  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }

  /* JCP target == instruction after the then-block (HALT at index 5) */
  ck_assert_int_eq(program->ir_code->ir[1].args[0], 5);
}
END_TEST

/*
 * if cond { let x := 1 } else { let y := 2 }
 * IR (indices):
 *  0 PUSHI  (cond)
 *  1 JCP -> 6     (false: jump to else)
 *  2 ALLOC 3 PUSHI 4 STORE   (then)
 *  5 JMP -> 9     (skip else)
 *  6 ALLOC 7 PUSHI 8 STORE   (else)
 *  9 HALT
 */

START_TEST(test_jkl_compile_if_with_else)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *iff = jkl_node_new(JKL_NODE_IF);
  iff->expr = make_int_node(1);
  iff->block = make_block1(jkl_decl_let_with_qword("x", 1));
  iff->block_else = make_block1(jkl_decl_let_with_qword("y", 2));

  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, iff);
  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t output[] = {
    JKL_IR_PUSHI,
    JKL_IR_JCP,
    JKL_IR_ALLOC, JKL_IR_PUSHI, JKL_IR_STORE,
    JKL_IR_JMP,
    JKL_IR_ALLOC, JKL_IR_PUSHI, JKL_IR_STORE,
    JKL_IR_HALT
  };

  ck_assert_int_eq(program->ir_code->n_irs, 10);
  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }

  /* JCP false-branch target == else start (index 6) */
  ck_assert_int_eq(program->ir_code->ir[1].args[0], 6);
  /* JMP after then == merge point (HALT at index 9) */
  ck_assert_int_eq(program->ir_code->ir[5].args[0], 9);
}
END_TEST

/*
 * Round-trip: compile -> save -> load -> compare instruction streams.
 */

START_TEST(test_jkl_ir_save_load_roundtrip)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, jkl_decl_let_with_qword("x", 42));
  program->ast_prog_root = block;

  jkl_compile(program);

  const char *path = "/tmp/jkl_roundtrip_test.bin";
  jkl_ir_code_save(program->ir_code, path);

  jkl_ir_code_t loaded;
  jkl_word_t r = jkl_ir_code_load(&loaded, path);
  ck_assert_int_eq(r, 0);
  ck_assert_int_eq(loaded.n_irs, program->ir_code->n_irs);

  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(loaded.ir[i].type, program->ir_code->ir[i].type);
    ck_assert_int_eq(loaded.ir[i].args[0], program->ir_code->ir[i].args[0]);
  }
}
END_TEST

/*
 * Compiler test suite
 */

Suite *jkl_compiler_suite()
{
  Suite *s = suite_create("jkl_compiler");

  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_jkl_program_new);
  tcase_add_test(tc_core, test_jkl_compile);
  tcase_add_test(tc_core, test_jkl_compile_check_ir);
  tcase_add_test(tc_core, test_jkl_compile_check_let_with_int);
  tcase_add_test(tc_core, test_jkl_compile_check_let_with_string);
  tcase_add_test(tc_core, test_jkl_compile_check_with_loop);
  tcase_add_test(tc_core, test_jkl_compile_if_without_else);
  tcase_add_test(tc_core, test_jkl_compile_if_with_else);
  tcase_add_test(tc_core, test_jkl_ir_save_load_roundtrip);
  suite_add_tcase(s, tc_core);

  return s;
}
