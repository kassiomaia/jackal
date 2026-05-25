/*
 * Integration tests that drive the real flex/bison front end.
 *
 * Codegen is post-order, so it is already order-correct for any AST; the
 * operator-precedence fix (#9) lives entirely in the grammar. The only way to
 * exercise it is to parse real source, so this harness compiles a snippet to a
 * temp file, runs yyparse()+jkl_compile(), and inspects the emitted IR.
 *
 * It doubles as a memory check: each parsed AST is released with the recursive
 * jkl_node_free() (#11) and the hash/symbol-table frees (#6/#7) are exercised
 * directly. Build with -fsanitize=address to catch leaks and invalid frees.
 *
 * Snippets here deliberately avoid `call`/`func` (bugs #3/#4 leak token strings,
 * out of scope for this milestone) so ASan output stays meaningful.
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
  jkl_node_free(p->ast_prog_root);
  jkl_hash_free(p->symbol_table);
  free(p->symbol_table);
  jkl_ir_code_free(p->ir_code);
  free(p->ir_code);
  free(p);
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
