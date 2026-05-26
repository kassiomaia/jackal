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

  /* the first declared variable resolves to slot 0 (ALLOC/STORE operand) */
  ck_assert_int_eq(program->ir_code->ir[0].args[0], 0);
  ck_assert_int_eq(program->ir_code->ir[2].args[0], 0);
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
 * let x := 1
 * let y := x      (x is a read -> must resolve to x's slot, not a name hash)
 * IR (indices / slots):
 *  0 ALLOC 0   1 PUSHI   2 STORE 0     (x in slot 0)
 *  3 ALLOC 1   4 LOAD 0  5 STORE 1     (y in slot 1; read of x -> slot 0)
 *  6 HALT
 */
START_TEST(test_jkl_compile_id_resolves_to_slot)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *let_x = jkl_decl_let_with_qword("x", 1);

  jkl_node_t *let_y = jkl_node_new(JKL_NODE_LET);
  let_y->id = jkl_node_new(JKL_NODE_ID);
  let_y->id->value.s = "y";
  let_y->expr = jkl_node_new(JKL_NODE_ID);
  let_y->expr->value.s = "x";

  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, let_x);
  jkl_node_append(block, let_y);
  program->ast_prog_root = block;

  jkl_word_t r = jkl_compile(program);
  ck_assert_int_eq(r, 0);

  jkl_ir_type_t output[] = {
    JKL_IR_ALLOC, JKL_IR_PUSHI, JKL_IR_STORE,
    JKL_IR_ALLOC, JKL_IR_LOAD, JKL_IR_STORE,
    JKL_IR_HALT
  };

  ck_assert_int_eq(program->ir_code->n_irs, 7);
  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }

  ck_assert_int_eq(program->ir_code->ir[0].args[0], 0); /* ALLOC x -> 0 */
  ck_assert_int_eq(program->ir_code->ir[3].args[0], 1); /* ALLOC y -> 1 */
  ck_assert_int_eq(program->ir_code->ir[4].args[0], 0); /* LOAD x  -> 0 */
  ck_assert_int_eq(program->ir_code->ir[5].args[0], 1); /* STORE y -> 1 */
}
END_TEST

/*
 * func inc(n) { return n }
 * inc 41
 * IR: top-level first (entry 0), HALT, then the hoisted func body.
 *  0 PUSHI 41        (call arg)
 *  1 CALL  entry,0,1 (internal: target = inc's entry, kind 0, nargs 1)
 *  2 HALT
 *  3 ALLOC s         (inc prologue: bind param)  <- entry
 *  4 STORE s
 *  5 LOAD  s         (return n)
 *  6 RET
 */
START_TEST(test_jkl_compile_call_func_return)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *param = jkl_node_new(JKL_NODE_PARAM);
  param->id = jkl_node_new(JKL_NODE_ID);
  param->id->value.s = "n";
  jkl_node_t *params = jkl_node_new(JKL_NODE_PARAMS);
  jkl_node_append(params, param);

  jkl_node_t *ret = jkl_node_new(JKL_NODE_RETURN);
  ret->expr = jkl_node_new(JKL_NODE_ID);
  ret->expr->value.s = "n";
  jkl_node_t *fbody = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(fbody, ret);

  jkl_node_t *func = jkl_node_new(JKL_NODE_FUNC);
  func->id = jkl_node_new(JKL_NODE_ID);
  func->id->value.s = "inc";
  func->params = params;
  func->block = fbody;

  jkl_node_t *call = jkl_node_new(JKL_NODE_CALL);
  call->id = jkl_node_new(JKL_NODE_ID);
  call->id->value.s = "inc";
  call->node = make_int_node(41);

  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, func);
  jkl_node_append(block, call);
  program->ast_prog_root = block;

  ck_assert_int_eq(jkl_compile(program), 0);

  jkl_ir_type_t output[] = {
    JKL_IR_PUSHI, JKL_IR_CALL, JKL_IR_HALT,
    JKL_IR_ALLOC, JKL_IR_STORE, JKL_IR_LOAD, JKL_IR_RET
  };
  ck_assert_int_eq(program->ir_code->n_irs, 7);
  for (int i = 0; i < program->ir_code->n_irs; i++) {
    ck_assert_int_eq(program->ir_code->ir[i].type, output[i]);
  }

  /* CALL is internal (kind 0), one arg, targeting the func entry (index 3) */
  ck_assert_int_eq(program->ir_code->ir[1].args[0], 3);
  ck_assert_int_eq(program->ir_code->ir[1].args[1], 0);
  ck_assert_int_eq(program->ir_code->ir[1].args[2], 1);
  /* prologue/body use the same param slot */
  ck_assert_int_eq(program->ir_code->ir[3].args[0], program->ir_code->ir[4].args[0]);
  ck_assert_int_eq(program->ir_code->ir[3].args[0], program->ir_code->ir[5].args[0]);
}
END_TEST

/*
 * puts "hi"   (puts has no func definition -> external/builtin call)
 *  0 LOAD ...           (string arg "hi" from bss)
 *  1 CALL off,1,1       (kind 1 external: target = bss offset of "puts")
 *  2 HALT
 */
START_TEST(test_jkl_compile_external_call)
{
  jkl_program_t *program = jkl_program_new();

  jkl_node_t *call = jkl_node_new(JKL_NODE_CALL);
  call->id = jkl_node_new(JKL_NODE_ID);
  call->id->value.s = "puts";
  call->node = jkl_node_new(JKL_NODE_STRING);
  call->node->value.s = "hi";

  jkl_node_t *block = jkl_node_new(JKL_NODE_BLOCK);
  jkl_node_append(block, call);
  program->ast_prog_root = block;

  ck_assert_int_eq(jkl_compile(program), 0);

  ck_assert_int_eq(program->ir_code->n_irs, 3);
  ck_assert_int_eq(program->ir_code->ir[0].type, JKL_IR_LOAD);
  ck_assert_int_eq(program->ir_code->ir[1].type, JKL_IR_CALL);
  ck_assert_int_eq(program->ir_code->ir[2].type, JKL_IR_HALT);

  /* external: kind 1, target = bss offset of the callee name */
  jkl_word_t off = jkl_string_hash("puts") % ((1 << (sizeof(jkl_word_t) * 8)) - 1);
  ck_assert_int_eq(program->ir_code->ir[1].args[1], 1);
  ck_assert_int_eq(program->ir_code->ir[1].args[0], off);
}
END_TEST

/*
 * Type system (behavioral): dispatch native methods via jkl_send, with no VM.
 * jkl_class_init must run exactly once (it registers an atexit cleanup).
 */
static void ensure_types(void)
{
  static int done = 0;
  if (!done) {
    jkl_class_init();
    done = 1;
  }
}

START_TEST(test_jkl_send_string)
{
  ensure_types();

  jkl_value_t len = jkl_send(jkl_string("hello"), "length", NULL, 0);
  ck_assert_int_eq(len.tag, JKL_T_INT);
  ck_assert_int_eq(len.as.i, 5);

  jkl_value_t up = jkl_send(jkl_string("abc"), "upcase", NULL, 0);
  ck_assert_int_eq(up.tag, JKL_T_STRING);
  ck_assert_int_eq(strcmp(up.as.s, "ABC"), 0);
  jkl_value_free(up);

  jkl_value_t rev = jkl_send(jkl_string("abc"), "reverse", NULL, 0);
  ck_assert_int_eq(strcmp(rev.as.s, "cba"), 0);
  jkl_value_free(rev);

  jkl_value_t arg = jkl_string("cd");
  jkl_value_t cat = jkl_send(jkl_string("ab"), "concat", &arg, 1);
  ck_assert_int_eq(strcmp(cat.as.s, "abcd"), 0);
  jkl_value_free(cat);

  jkl_value_t empty = jkl_send(jkl_string(""), "empty?", NULL, 0);
  ck_assert_int_eq(empty.tag, JKL_T_BOOL);
  ck_assert_int_eq(empty.as.i, 1);
}
END_TEST

START_TEST(test_jkl_send_integer)
{
  ensure_types();

  jkl_value_t a = jkl_send(jkl_int(-3), "abs", NULL, 0);
  ck_assert_int_eq(a.tag, JKL_T_INT);
  ck_assert_int_eq(a.as.i, 3);

  ck_assert_int_eq(jkl_send(jkl_int(4), "even?", NULL, 0).as.i, 1);
  ck_assert_int_eq(jkl_send(jkl_int(4), "odd?", NULL, 0).as.i, 0);
  ck_assert_int_eq(jkl_send(jkl_int(4), "succ", NULL, 0).as.i, 5);

  jkl_value_t s = jkl_send(jkl_int(-3), "to_s", NULL, 0);
  ck_assert_int_eq(s.tag, JKL_T_STRING);
  ck_assert_int_eq(strcmp(s.as.s, "-3"), 0);
  jkl_value_free(s);
}
END_TEST

START_TEST(test_jkl_send_boolean)
{
  ensure_types();

  jkl_value_t n = jkl_send(jkl_bool(1), "not", NULL, 0);
  ck_assert_int_eq(n.tag, JKL_T_BOOL);
  ck_assert_int_eq(n.as.i, 0);

  jkl_value_t s = jkl_send(jkl_bool(0), "to_s", NULL, 0);
  ck_assert_int_eq(strcmp(s.as.s, "false"), 0);
  jkl_value_free(s);
}
END_TEST

/*
 * Arrays — data-manipulation methods (dispatched via jkl_send).
 */
START_TEST(test_jkl_arr_basic)
{
  ensure_types();
  jkl_array_t *arr = jkl_array_new();
  jkl_array_push(arr, jkl_int(10));
  jkl_array_push(arr, jkl_int(20));
  jkl_array_push(arr, jkl_int(30));
  jkl_value_t a = jkl_array_value(arr);

  ck_assert_int_eq(jkl_send(a, "length", NULL, 0).as.i, 3);

  jkl_value_t one = jkl_int(1);
  ck_assert_int_eq(jkl_send(a, "at", &one, 1).as.i, 20);

  jkl_value_t popped = jkl_send(a, "pop", NULL, 0);
  ck_assert_int_eq(popped.as.i, 30);
  ck_assert_int_eq(jkl_send(a, "length", NULL, 0).as.i, 2);

  ck_assert_int_eq(jkl_send(a, "empty?", NULL, 0).as.i, 0);

  jkl_value_t sep = jkl_string(", ");
  jkl_value_t joined = jkl_send(a, "join", &sep, 1);
  ck_assert_int_eq(joined.tag, JKL_T_STRING);
  ck_assert_int_eq(strcmp(joined.as.s, "10, 20"), 0);
  jkl_value_free(joined);

  jkl_value_t rev = jkl_send(a, "reverse", NULL, 0);
  ck_assert_int_eq(rev.tag, JKL_T_ARRAY);
  ck_assert_int_eq(jkl_send(rev, "at", &one, 1).as.i, 10); /* rev = [20, 10] */
  jkl_value_free(rev);

  jkl_value_free(a);
}
END_TEST

/* Iterators driven through the block evaluator, with the block AST built by
 * hand (no parser involved). Proves end-to-end: jkl_send -> native method ->
 * jkl_block_call -> jkl_eval_expr -> jkl_send (for x.succ). */
START_TEST(test_jkl_arr_map_block)
{
  ensure_types();

  /* block AST: { |x| x.succ } */
  jkl_node_t *param = jkl_node_new(JKL_NODE_PARAM);
  param->id = jkl_node_new(JKL_NODE_ID);
  param->id->value.s = "x";
  jkl_node_t *params = jkl_node_new(JKL_NODE_PARAMS);
  jkl_node_append(params, param);

  jkl_node_t *recv = jkl_node_new(JKL_NODE_ID);
  recv->value.s = "x";
  jkl_node_t *body = jkl_node_new(JKL_NODE_METHOD_CALL);
  body->node = recv;
  body->id = jkl_node_new(JKL_NODE_ID);
  body->id->value.s = "succ";

  jkl_block_t blk_storage = { .params = params, .expr = body };
  jkl_value_t blk_val;
  blk_val.tag = JKL_T_BLOCK;
  blk_val.owned = 0;
  blk_val.as.obj = &blk_storage;

  jkl_array_t *arr = jkl_array_new();
  jkl_array_push(arr, jkl_int(1));
  jkl_array_push(arr, jkl_int(2));
  jkl_array_push(arr, jkl_int(3));
  jkl_value_t a = jkl_array_value(arr);

  jkl_value_t result = jkl_send(a, "map", &blk_val, 1);
  ck_assert_int_eq(result.tag, JKL_T_ARRAY);
  ck_assert_int_eq(jkl_send(result, "length", NULL, 0).as.i, 3);

  jkl_value_t i0 = jkl_int(0), i1 = jkl_int(1), i2 = jkl_int(2);
  ck_assert_int_eq(jkl_send(result, "at", &i0, 1).as.i, 2);
  ck_assert_int_eq(jkl_send(result, "at", &i1, 1).as.i, 3);
  ck_assert_int_eq(jkl_send(result, "at", &i2, 1).as.i, 4);

  jkl_value_free(result);
  jkl_value_free(a);
  /* AST nodes leak in the non-ASan compiler target (same pattern as the other
   * jkl_send tests; freeing would crash on the string-literal value.s fields). */
}
END_TEST

/* reduce with the canonical "free previous acc" pattern. */
START_TEST(test_jkl_arr_reduce_block)
{
  ensure_types();

  /* block AST: { |acc, x| acc + x } */
  jkl_node_t *p1 = jkl_node_new(JKL_NODE_PARAM);
  p1->id = jkl_node_new(JKL_NODE_ID); p1->id->value.s = "acc";
  jkl_node_t *p2 = jkl_node_new(JKL_NODE_PARAM);
  p2->id = jkl_node_new(JKL_NODE_ID); p2->id->value.s = "x";
  jkl_node_t *params = jkl_node_new(JKL_NODE_PARAMS);
  jkl_node_append(params, p1);
  jkl_node_append(params, p2);

  jkl_node_t *l = jkl_node_new(JKL_NODE_ID); l->value.s = "acc";
  jkl_node_t *r = jkl_node_new(JKL_NODE_ID); r->value.s = "x";
  jkl_node_t *body = jkl_node_binop(l, JKL_OP_PLUS, r);

  jkl_block_t blk_storage = { .params = params, .expr = body };
  jkl_value_t blk_val = { .tag = JKL_T_BLOCK, .owned = 0, .as.obj = &blk_storage };

  jkl_array_t *arr = jkl_array_new();
  jkl_array_push(arr, jkl_int(1));
  jkl_array_push(arr, jkl_int(2));
  jkl_array_push(arr, jkl_int(3));
  jkl_array_push(arr, jkl_int(4));
  jkl_value_t a = jkl_array_value(arr);

  jkl_value_t argv[2] = { jkl_int(0), blk_val };
  jkl_value_t sum = jkl_send(a, "reduce", argv, 2);
  ck_assert_int_eq(sum.tag, JKL_T_INT);
  ck_assert_int_eq(sum.as.i, 10);

  jkl_value_free(a);
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
  tcase_add_test(tc_core, test_jkl_compile_id_resolves_to_slot);
  tcase_add_test(tc_core, test_jkl_compile_call_func_return);
  tcase_add_test(tc_core, test_jkl_compile_external_call);
  tcase_add_test(tc_core, test_jkl_send_string);
  tcase_add_test(tc_core, test_jkl_send_integer);
  tcase_add_test(tc_core, test_jkl_send_boolean);
  tcase_add_test(tc_core, test_jkl_arr_basic);
  tcase_add_test(tc_core, test_jkl_arr_map_block);
  tcase_add_test(tc_core, test_jkl_arr_reduce_block);
  tcase_add_test(tc_core, test_jkl_compile_check_with_loop);
  tcase_add_test(tc_core, test_jkl_compile_if_without_else);
  tcase_add_test(tc_core, test_jkl_compile_if_with_else);
  tcase_add_test(tc_core, test_jkl_ir_save_load_roundtrip);
  suite_add_tcase(s, tc_core);

  return s;
}
