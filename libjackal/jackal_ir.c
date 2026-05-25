#include <jackal.h>
#include <fcntl.h>

#define MIN_IR_CODE_SIZE 1024

static jkl_byte_t jkl_host_endianness(void)
{
  jkl_word_t w = 1;
  return (*(jkl_byte_t *)&w) ? JKL_IR_ENDIAN_LITTLE : JKL_IR_ENDIAN_BIG;
}

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

void jkl_ir_code_patch(jkl_ir_code_t *ir_code, jkl_word_t idx,
                       jkl_byte_t argn, jkl_qqword_t value)
{
  ir_code->ir[idx].args[(int)argn] = value;
}

jkl_word_t jkl_ir_code_save(jkl_ir_code_t *ir_code, const char *filename)
{
  jkl_note("jkl_ir", "saving ir code to %s", filename);
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    jkl_error("jkl_ir", "cannot open file for writing");
  }

  jkl_qqword_t bss_len = (1 << (sizeof(jkl_word_t) * 8)) - 1;

  jkl_ir_file_header_t hdr;
  memset(&hdr, 0, sizeof(hdr));
  hdr.magic[0] = JKL_IR_MAGIC0;
  hdr.magic[1] = JKL_IR_MAGIC1;
  hdr.magic[2] = JKL_IR_MAGIC2;
  hdr.magic[3] = JKL_IR_MAGIC3;
  hdr.version = JKL_IR_FORMAT_VERSION;
  hdr.endianness = jkl_host_endianness();
  hdr.ir_struct_size = (jkl_byte_t)sizeof(jkl_ir_t);
  hdr.n_irs = ir_code->n_irs;
  hdr.bss_len = bss_len;

  ssize_t n;
  ssize_t ir_bytes = (ssize_t)(sizeof(jkl_ir_t) * ir_code->n_irs);

  n = write(fd, &hdr, sizeof(hdr));
  if (n != (ssize_t)sizeof(hdr)) {
    jkl_error("jkl_ir", "cannot write header");
  }

  n = write(fd, ir_code->ir, (size_t)ir_bytes);
  if (n != ir_bytes) {
    jkl_error("jkl_ir", "cannot write instructions");
  }

  n = write(fd, ir_code->bss, (size_t)bss_len);
  if (n != (ssize_t)bss_len) {
    jkl_error("jkl_ir", "cannot write data section");
  }

  close(fd);

  return 0;
}

jkl_word_t jkl_ir_code_load(jkl_ir_code_t *ir_code, const char *filename)
{
  jkl_note("jkl_ir", "loading ir code from %s", filename);
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    jkl_error("jkl_ir", "cannot open file for reading");
  }

  jkl_ir_file_header_t hdr;
  ssize_t n = read(fd, &hdr, sizeof(hdr));
  if (n != (ssize_t)sizeof(hdr)) {
    close(fd);
    jkl_error("jkl_ir", "cannot read header");
  }

  if (hdr.magic[0] != JKL_IR_MAGIC0 || hdr.magic[1] != JKL_IR_MAGIC1 ||
      hdr.magic[2] != JKL_IR_MAGIC2 || hdr.magic[3] != JKL_IR_MAGIC3) {
    close(fd);
    jkl_error("jkl_ir", "bad magic (not a jackal bytecode file)");
  }
  if (hdr.version > JKL_IR_FORMAT_VERSION) {
    close(fd);
    jkl_error("jkl_ir", "unsupported format version");
  }
  if (hdr.ir_struct_size != (jkl_byte_t)sizeof(jkl_ir_t)) {
    close(fd);
    jkl_error("jkl_ir", "incompatible instruction size");
  }
  if (hdr.endianness != jkl_host_endianness()) {
    close(fd);
    jkl_error("jkl_ir", "incompatible endianness");
  }

  jkl_ir_code_init(ir_code, (jkl_word_t)hdr.n_irs);
  ir_code->n_irs = hdr.n_irs;

  ssize_t ir_bytes = (ssize_t)(sizeof(jkl_ir_t) * hdr.n_irs);
  n = read(fd, ir_code->ir, (size_t)ir_bytes);
  if (n != ir_bytes) {
    close(fd);
    jkl_error("jkl_ir", "cannot read instructions");
  }

  n = read(fd, ir_code->bss, (size_t)hdr.bss_len);
  if (n != (ssize_t)hdr.bss_len) {
    close(fd);
    jkl_error("jkl_ir", "cannot read data section");
  }

  close(fd);

  return 0;
}

void jkl_ir_store_string(jkl_ir_code_t *ir_code, jkl_string_t *string)
{
  if (jkl_string_len((char *)string) == 0) {
    return;
  }

  jkl_word_t hash = jkl_string_hash((char *)string);
  jkl_note("jkl_ir", "storing string `%s` with hash %d", string, hash);
  jkl_byte_t *bss = ir_code->bss;
  jkl_word_t pos = hash % ((1 << (sizeof(jkl_word_t) * 8)) - 1);
  jkl_note("jkl_ir", "storing string at bss[%d]", pos);

  memcpy(bss + pos, string, jkl_string_len((char *)string));
}

void jkl_ir_code_free(jkl_ir_code_t *ir_code)
{
  free(ir_code->ir);
  ir_code->n_irs = 0;
}
