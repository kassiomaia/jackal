#include <jackal.h>

#define MAX_NESTED_BLOCKS 1024

jkl_node_t *cc;

struct {
  jkl_node_t* frames[MAX_NESTED_BLOCKS];
  int pos;
} context;

jkl_node_t* jkl_get_context(jkl_program_t *program) {
  if (context.pos == 0) {
    return NULL;
  }

  jkl_log("jkl_parser", "context.pos = %d", context.pos);
  return context.frames[context.pos - 1];
}

void jkl_push_context(jkl_program_t *program, jkl_node_t *node) {
  if (jkl_get_context(program) != NULL) {
    jkl_node_t* parent = jkl_get_context(program);
    node->parent = parent;
  }

  jkl_log("jkl_parser", "pushing context");
  context.frames[context.pos] = node;
  context.pos++;
}

jkl_node_t* jkl_pop_context(jkl_program_t *program) {
  context.pos--;
  jkl_log("jkl_parser", "popping context");
  return context.frames[context.pos];
}

void jkl_ensure_empty_contexts() {
  if (context.pos != 0)
    jkl_error("jkl_error", "context stack is not empty");
}
