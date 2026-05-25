/*
 * Integration tests that drive the real flex/bison front end.
 *
 * Codegen is post-order, so it is already order-correct for any AST; the
 * operator-precedence fix (#9) lives entirely in the grammar. The only way to
 * exercise it is to parse real source, so this harness compiles a snippet to a
 * temp file, runs yyparse()+jkl_compile(), and inspects the emitted IR.
 *
 * It doubles as a memory check: each parsed program is released with
 * jkl_program_free() (recursive AST free #11 + symbol table), so leaks or invalid
 * frees surface. Build with -fsanitize=address to catch them.
 *
 * Coverage includes precedence (#9), variable slot resolution (#3 symbol table),
 * and now call/func/return (#4): internal calls into hoisted function bodies and
 * external/builtin calls (a callee with no `func` definition, e.g. `puts`).
 */
#include <jackal.h>

extern FILE *yyin;
extern int yyparse(void);
extern void yyrestart(FILE *);
extern jkl_program_t *program;

static int failures = 0;

#define CHECK(cond, msg) do {                                                  \
  if (cond) {                                                                  \
    printf("  ok   - %s\n", msg);                                             \
  } else {                                                                     \
    printf("  FAIL - %s  (%s:%d)\n", msg, __FILE__, __LINE__);                \
    failures++;                                                                \
  }                                                                            \
} while (0)

static jkl_program_t *compile_snippet(const char *src)
{
  char path[] = "/tmp/jkl_prec_XXXXXX";
  int fd = mkstemp(path);
  if (fd < 0) {
    jkl_error("jkl_test", "mkstemp failed");
  }
  FILE *f = fdopen(fd, "w");
  fputs(src, f);
  fclose(f);

  yyin = fopen(path, "r");
  if (yyin == NULL) {
    jkl_error("jkl_test", "cannot reopen snippet");
  }
  yyrestart(yyin);

  if (yyparse() != 0) {
    jkl_error("jkl_test", "parse failed for: %s", src);
  }
  fclose(yyin);
  unlink(path);

  jkl_compile(program);
  return program;
}

static void free_program(jkl_program_t *p)
{
  jkl_program_free(p);
}

/* Index of the first instruction with the given opcode, or -1. */
static int first_op(jkl_program_t *p, jkl_ir_type_t op)
{
  for (int i = 0; i < p->ir_code->n_irs; i++) {
    if (p->ir_code->ir[i].type == op) {
      return i;
    }
  }
  return -1;
}

/* Compare the emitted opcode stream against an expected sequence. */
static int ir_seq_eq(jkl_program_t *p, const jkl_ir_type_t *want, int n)
{
  if (p->ir_code->n_irs != n) {
    return 0;
  }
  for (int i = 0; i < n; i++) {
    if (p->ir_code->ir[i].type != want[i]) {
      return 0;
    }
  }
  return 1;
}

/* `1 + 2 * 3` must group as 1 + (2*3): MUL emitted before ADD. */
static void test_precedence_mul_over_add(void)
{
  printf("test: precedence (* binds tighter than +)\n");
  jkl_program_t *p = compile_snippet("let r := 1 + 2 * 3\n");
  jkl_ir_type_t want[] = {
    JKL_IR_ALLOC,
    JKL_IR_PUSHI, JKL_IR_PUSHI, JKL_IR_PUSHI,
    JKL_IR_MUL, JKL_IR_ADD,
    JKL_IR_STORE,
    JKL_IR_HALT,
  };
  CHECK(ir_seq_eq(p, want, 8), "1 + 2 * 3 => PUSH PUSH PUSH MUL ADD");
  free_program(p);
}

/* `1 - 2 - 3` is left-associative: (1-2)-3 => SUB before the third operand. */
static void test_left_associativity(void)
{
  printf("test: left associativity of -\n");
  jkl_program_t *p = compile_snippet("let r := 1 - 2 - 3\n");
  jkl_ir_type_t want[] = {
    JKL_IR_ALLOC,
    JKL_IR_PUSHI, JKL_IR_PUSHI, JKL_IR_SUB,
    JKL_IR_PUSHI, JKL_IR_SUB,
    JKL_IR_STORE,
    JKL_IR_HALT,
  };
  CHECK(ir_seq_eq(p, want, 8), "1 - 2 - 3 => PUSH PUSH SUB PUSH SUB");
  free_program(p);
}

/* Arithmetic binds tighter than comparison: 1 + 2 == 3 => (1+2) == 3. */
static void test_arith_over_compare(void)
{
  printf("test: arithmetic binds tighter than ==\n");
  jkl_program_t *p = compile_snippet("let r := 1 + 2 == 3\n");
  jkl_ir_type_t want[] = {
    JKL_IR_ALLOC,
    JKL_IR_PUSHI, JKL_IR_PUSHI, JKL_IR_ADD,
    JKL_IR_PUSHI, JKL_IR_EQL,
    JKL_IR_STORE,
    JKL_IR_HALT,
  };
  CHECK(ir_seq_eq(p, want, 8), "1 + 2 == 3 => PUSH PUSH ADD PUSH EQL");
  free_program(p);
}

/* A variable read resolves to the slot its `let` allocated (not a name hash). */
static void test_id_resolves_to_slot(void)
{
  printf("test: ID reads resolve to declared slots\n");
  jkl_program_t *p = compile_snippet("let a := 5\nlet b := a + 1\n");
  jkl_ir_type_t want[] = {
    JKL_IR_ALLOC, JKL_IR_PUSHI, JKL_IR_STORE,        /* a := 5   -> slot 0 */
    JKL_IR_ALLOC, JKL_IR_LOAD, JKL_IR_PUSHI,         /* b := a+1 -> slot 1 */
    JKL_IR_ADD, JKL_IR_STORE,
    JKL_IR_HALT,
  };
  CHECK(ir_seq_eq(p, want, 9), "let a; let b := a + 1 opcode stream");
  if (p->ir_code->n_irs == 9) {
    CHECK(p->ir_code->ir[0].args[0] == 0, "ALLOC a -> slot 0");
    CHECK(p->ir_code->ir[3].args[0] == 1, "ALLOC b -> slot 1");
    CHECK(p->ir_code->ir[4].args[0] == 0, "LOAD a  -> slot 0 (read resolves)");
    CHECK(p->ir_code->ir[7].args[0] == 1, "STORE b -> slot 1");
  }
  free_program(p);
}

/* A call to a user-defined func resolves to an internal CALL into the hoisted body. */
static void test_internal_call(void)
{
  printf("test: internal call into a hoisted function body\n");
  jkl_program_t *p = compile_snippet(
    "func inc(n) { return n }\nlet x := 41\ninc x\n");
  int call = first_op(p, JKL_IR_CALL);
  int halt = first_op(p, JKL_IR_HALT);
  CHECK(call >= 0 && halt >= 0, "emits a CALL and a HALT");
  if (call >= 0 && halt >= 0) {
    CHECK(p->ir_code->ir[call].args[1] == 0, "CALL kind == 0 (internal)");
    CHECK(p->ir_code->ir[call].args[0] > halt, "CALL target is in the hoisted region (after HALT)");
    CHECK(first_op(p, JKL_IR_RET) > halt, "the function body ends in RET after HALT");
  }
  free_program(p);
}

/* A call to an undefined name compiles as an external/builtin CALL (no error). */
static void test_external_call(void)
{
  printf("test: external/builtin call (undefined callee)\n");
  jkl_program_t *p = compile_snippet("puts \"x\"\n");
  int call = first_op(p, JKL_IR_CALL);
  CHECK(call >= 0, "emits a CALL");
  if (call >= 0) {
    CHECK(p->ir_code->ir[call].args[1] == 1, "CALL kind == 1 (external)");
  }
  free_program(p);
}

/* Parse a tree spanning many node types, then free it (ASan proves #11). */
static void test_recursive_free_real_ast(void)
{
  printf("test: recursive AST free on real parser output\n");
  jkl_program_t *p = compile_snippet(
    "let a := 1 + 2 * 3\n"
    "let s := \"hello\"\n"
    "if a == 7 { let b := a - 1 } else { let c := 0 }\n"
    "loop { let d := 1 }\n");
  CHECK(p->ast_prog_root != NULL, "snippet parsed into an AST");
  free_program(p); /* ASan: no leaks, no invalid frees */
  CHECK(1, "freed BLOCK/LET/ID/STRING/BINOP/IF/LOOP without error");
}

/* Direct exercise of the two free fixes (#6 hash, #7 symbol table). */
static void test_container_frees(void)
{
  printf("test: hash + symbol-table free\n");
  jkl_hash_tbl *h = jkl_hash_new();
  jkl_hash_free(h);
  free(h);
  CHECK(1, "jkl_hash_free on an empty table");

  jkl_symbol_table_t *t = jkl_symbol_table_new();
  jkl_symbol_table_add(t, "x", JKL_SYMBOL_LET);
  jkl_symbol_table_free(t);
  CHECK(1, "jkl_symbol_table_free frees the whole block");
}

int main(void)
{
  printf("==============================================================\n");
  test_precedence_mul_over_add();
  test_left_associativity();
  test_arith_over_compare();
  test_id_resolves_to_slot();
  test_internal_call();
  test_external_call();
  test_recursive_free_real_ast();
  test_container_frees();
  printf("==============================================================\n");
  if (failures == 0) {
    printf("Jackal integration tests passed!\n");
    return EXIT_SUCCESS;
  }
  printf("Jackal integration tests failed (%d)!\n", failures);
  return EXIT_FAILURE;
}
