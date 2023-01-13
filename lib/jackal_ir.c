#include <jackal.h>
#include <fcntl.h>

#define MIN_IR_CODE_SIZE 1024

void jkl_ir_code_init(jkl_ir_code_t *ir_code, jkl_word_t size)
{
  if (size < MIN_IR_CODE_SIZE) {
    size = MIN_IR_CODE_SIZE;
  }

  ir_code->ir = (jkl_ir_t *)malloc(sizeof(jkl_ir_t) * size);
  if (ir_code->ir == NULL) {
    jkl_error("jkl_ir", "cannot allocate ir code");
  }

  ir_code->n_irs = 0;

  for (jkl_word_t i = 0; i < size; i++) {
    ir_code->ir[i] = JKL_EMIT_IR(JKL_IR_NOPE, 0, 0, 0);
  }

  for (jkl_word_t i = 0; i < sizeof(jkl_word_t); i++) {
    ir_code->bss[i] = 0;
  }
}

jkl_word_t jkl_ir_code_push(jkl_ir_code_t *ir_code, jkl_ir_t ir)
{
  // realloc if necessary
  if (ir_code->n_irs % MIN_IR_CODE_SIZE == 0) {
    jkl_word_t new_size = ir_code->n_irs + MIN_IR_CODE_SIZE;
    ir_code->ir = (jkl_ir_t *)realloc(ir_code->ir, sizeof(jkl_ir_t) * new_size);
    if (ir_code->ir == NULL) {
      jkl_error("jkl_ir", "cannot reallocate ir code");
    }

    for (jkl_word_t i = ir_code->n_irs; i < new_size; i++) {
      ir_code->ir[i] = JKL_EMIT_IR(JKL_IR_NOPE, 0, 0, 0);
    }
  }

  ir_code->ir[ir_code->n_irs] = ir;
  ir_code->n_irs++;

  return ir_code->n_irs - 1;
}

jkl_word_t jkl_ir_code_save(jkl_ir_code_t *ir_code, const char *filename)
{
  jkl_note("jkl_ir", "saving ir code to %s", filename);
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    jkl_error("jkl_ir", "cannot open file for writing");
  }

  jkl_word_t n_bytes = write(fd, ir_code->ir, sizeof(jkl_ir_t) * ir_code->n_irs);
  if (n_bytes == -1) {
    jkl_error("jkl_ir", "cannot write to file");
  }

  n_bytes = write(fd, &ir_code->bss,
                  sizeof(jkl_byte_t) * (1 << (sizeof(jkl_word_t) * 8) - 1));
  if (n_bytes == -1) {
    jkl_error("jkl_ir", "cannot write to file");
  }

  close(fd);
}

void jkl_ir_store_string(jkl_ir_code_t *ir_code, jkl_string_t *string)
{
  if (jkl_string_len(string) == 0) {
    return;
  }

  jkl_word_t hash = jkl_string_hash(string);
  jkl_note("jkl_ir", "storing string `%s` with hash %d", string, hash);
  jkl_byte_t *bss = ir_code->bss;
  jkl_word_t pos = hash % (1 << (sizeof(jkl_word_t) * 8) - 1);
  jkl_note("jkl_ir", "storing string at bss[%d]", pos);

  memcpy(bss + pos, string, jkl_string_len(string));
}

void jkl_ir_code_free(jkl_ir_code_t *ir_code)
{
  free(ir_code->ir);
  ir_code->n_irs = 0;
}
