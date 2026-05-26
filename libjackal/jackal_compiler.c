#include <jackal.h>

#define invariant(x, msg) \
  if (x) \
    jkl_error("jkl_compiler", "invariant failed: " msg);

jkl_program_t *jkl_program_new()
{
  jkl_program_t *program = malloc(sizeof(jkl_program_t));
  if (program == NULL) {
    jkl_error("jkl_compiler", "cannot allocate memory for program");
  }

  memset(program, 0, sizeof(jkl_program_t));

  jkl_program_init(program);
  return program;
}

void jkl_program_init(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  program->ast_prog_root = NULL;
  program->n_errors = 0;
  program->n_warnings = 0;
  program->n_notes = 0;
  program->n_ids = 0;
  program->n_funcs = 0;
  program->symbol_table = jkl_symbol_table_new();
  program->ir_code = malloc(sizeof(jkl_ir_code_t));
  if (program->ir_code == NULL) {
    jkl_error("jkl_compiler", "cannot allocate memory for ir code");
  }

  program->ir_code->ir = malloc(sizeof(jkl_ir_t) * 1024);
  if (program->ir_code->ir == NULL) {
    jkl_error("jkl_compiler", "cannot allocate memory for ir code");
  }

  program->ir_code->n_irs = 0;

  program->func_queue = NULL;
  program->n_func_queue = 0;
  program->func_queue_cap = 0;
  program->call_fixups = NULL;
  program->n_fixups = 0;
  program->fixups_cap = 0;
  program->blocks = NULL;
  program->n_blocks = 0;
  program->blocks_cap = 0;
}

void jkl_program_free(jkl_program_t *program)
{
  if (program == NULL) {
    return;
  }

  jkl_node_free(program->ast_prog_root);
  jkl_symbol_table_free(program->symbol_table);

  if (program->ir_code != NULL) {
    jkl_ir_code_free(program->ir_code);
    free(program->ir_code);
  }

  free(program->func_queue);
  free(program->call_fixups);
  for (size_t i = 0; i < program->n_blocks; i++) {
    free(program->blocks[i]);  /* the jkl_block_t header; its AST is program-owned */
  }
  free(program->blocks);
  free(program);
}

jkl_word_t jkl_get_idx(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  return program->n_ids++;
}

jkl_word_t jkl_emit_expr_op(jkl_program_t *program, jkl_op_t op)
{
  invariant(program == NULL, "program is NULL");

  switch (op) {
    case JKL_OP_EQL: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_EQL, 0, 0, 0));
      break;
    }
    case JKL_OP_NEQ: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_NEQ, 0, 0, 0));
      break;
    }
    case JKL_OP_GT: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_GT, 0, 0, 0));
      break;
    }
    case JKL_OP_LT: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_LT, 0, 0, 0));
      break;
    }
    case JKL_OP_GTE: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_GTE, 0, 0, 0));
      break;
    }
    case JKL_OP_LTE: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_LTE, 0, 0, 0));
      break;
    }
    case JKL_OP_PLUS: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_ADD, 0, 0, 0));
      break;
    }
    case JKL_OP_MINUS: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_SUB, 0, 0, 0));
      break;
    }
    case JKL_OP_MUL: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_MUL, 0, 0, 0));
      break;
    }
    case JKL_OP_DIV: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_DIV, 0, 0, 0));
      break;
    }
    case JKL_OP_MOD: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_MOD, 0, 0, 0));
      break;
    }
    case JKL_OP_AND: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_AND, 0, 0, 0));
      break;
    }
    case JKL_OP_OR: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_OR, 0, 0, 0));
      break;
    }
    case JKL_OP_NOT: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_NOT, 0, 0, 0));
      break;
    }
    default:
      jkl_error("jkl_compiler", "invlaid expr op");
  }

  return 0;
}

/* Write a NUL-terminated name into the data section and return its offset.
 * Shared by external CALL resolution and SEND lowering. */
static jkl_word_t jkl_bss_intern(jkl_program_t *program, jkl_string_t name)
{
  jkl_word_t len = jkl_string_len(name);
  jkl_word_t off = jkl_string_hash(name) % ((1 << (sizeof(jkl_word_t) * 8)) - 1);
  jkl_ir_store_string(program->ir_code, (jkl_string_t *)name);
  if ((jkl_word_t)(off + len) < ((1 << (sizeof(jkl_word_t) * 8)) - 1)) {
    program->ir_code->bss[off + len] = 0;
  }
  return off;
}

jkl_word_t jkl_compile_expr(jkl_program_t *program, jkl_node_t *expr)
{
  invariant(program == NULL, "program is NULL");
  invariant(expr == NULL, "block is NULL");

  switch (expr->type) {
    case JKL_NODE_INT: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_PUSHI, expr->value.i, 0,
                       0));
      break;
    }
    case JKL_NODE_FLOAT: {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_PUSHF, expr->value.f, 0,
                       0));
      break;
    }
    case JKL_NODE_STRING: {
      jkl_word_t hash = jkl_string_hash(expr->value.s);
      jkl_ir_store_string(program->ir_code, (jkl_string_t *)expr->value.s);
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_LOAD, hash,
                       hash + jkl_string_len(expr->value.s), 0));
      break;
    }
    case JKL_NODE_ID: {
      jkl_symbol_t *sym = jkl_symbol_table_get(program->symbol_table,
                                               expr->value.s);
      if (sym == NULL) {
        jkl_error("jkl_compiler", "undeclared identifier '%s'", expr->value.s);
      }
      jkl_ir_code_push(program->ir_code,
                       JKL_EMIT_IR(JKL_IR_LOAD, sym->slot, 0, 0));
      break;
    }
    case JKL_NODE_BINOP: {
      jkl_node_t *lhs = expr->binop.left;
      jkl_node_t *rhs = expr->binop.right;

      jkl_compile_expr(program, lhs);
      jkl_compile_expr(program, rhs);
      jkl_emit_expr_op(program, expr->binop.op);
      break;
    }
    case JKL_NODE_BOOL: {
      jkl_ir_code_push(program->ir_code,
                       JKL_EMIT_IR(JKL_IR_PUSHB, expr->value.i ? 1 : 0, 0, 0));
      break;
    }
    case JKL_NODE_METHOD_CALL: {
      /* receiver, then paren-args left-to-right, then (if present) the trailing
       * block as the LAST positional arg, then SEND name_off, argc. Block-last
       * matches Ruby's implicit-block convention and gives iterator methods a
       * predictable signature: each(blk), reduce(init, blk). */
      jkl_compile_expr(program, expr->node);
      jkl_word_t argc = 0;
      if (expr->params != NULL) {
        for (jkl_word_t i = 0; i < expr->params->compound.n_nodes; i++) {
          jkl_compile_expr(program, expr->params->compound.nodes[i]);
          argc++;
        }
      }
      if (expr->block_arg != NULL) {
        jkl_compile_expr(program, expr->block_arg);
        argc++;
      }
      jkl_word_t off = jkl_bss_intern(program, expr->id->value.s);
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_SEND, off, argc, 0));
      break;
    }
    case JKL_NODE_ARRAY_LIT: {
      for (jkl_word_t i = 0; i < expr->compound.n_nodes; i++) {
        jkl_compile_expr(program, expr->compound.nodes[i]);
      }
      jkl_ir_code_push(program->ir_code,
                       JKL_EMIT_IR(JKL_IR_NEWARR, expr->compound.n_nodes, 0, 0));
      break;
    }
    case JKL_NODE_BLOCK_LIT: {
      /* Register this block in the program's in-process block table and emit
       * PUSHBLK with its index. The block AST is owned by program->ast_prog_root;
       * the jkl_block_t header is freed in jkl_program_free. */
      if (program->n_blocks == program->blocks_cap) {
        program->blocks_cap = program->blocks_cap ? program->blocks_cap * 2 : 4;
        program->blocks = realloc(program->blocks,
                                  program->blocks_cap * sizeof(jkl_block_t *));
        if (program->blocks == NULL) {
          jkl_error("jkl_compiler", "cannot grow block table");
        }
      }
      jkl_block_t *blk = malloc(sizeof(jkl_block_t));
      if (blk == NULL) jkl_error("jkl_compiler", "out of memory for block");
      blk->params = expr->params;
      blk->expr = expr->expr;
      jkl_word_t idx = (jkl_word_t)program->n_blocks;
      program->blocks[program->n_blocks++] = blk;
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_PUSHBLK, idx, 0, 0));
      break;
    }
    default: {
      jkl_error("jkl_compiler", "invalid expression node");
    }
  }

  return 0;
}

static void jkl_func_queue_push(jkl_program_t *program, jkl_node_t *func)
{
  if (program->n_func_queue == program->func_queue_cap) {
    program->func_queue_cap = program->func_queue_cap ? program->func_queue_cap * 2 : 8;
    program->func_queue = realloc(program->func_queue,
                                  program->func_queue_cap * sizeof(jkl_node_t *));
    if (program->func_queue == NULL) {
      jkl_error("jkl_compiler", "cannot grow func queue");
    }
  }
  program->func_queue[program->n_func_queue++] = func;
}

static void jkl_call_fixup_push(jkl_program_t *program, jkl_word_t idx,
                                jkl_string_t name)
{
  if (program->n_fixups == program->fixups_cap) {
    program->fixups_cap = program->fixups_cap ? program->fixups_cap * 2 : 8;
    program->call_fixups = realloc(program->call_fixups,
                                   program->fixups_cap * sizeof(jkl_call_fixup_t));
    if (program->call_fixups == NULL) {
      jkl_error("jkl_compiler", "cannot grow call fixups");
    }
  }
  program->call_fixups[program->n_fixups].idx = idx;
  program->call_fixups[program->n_fixups].name = name;
  program->n_fixups++;
}

jkl_word_t jkl_compile_block(jkl_program_t *program, jkl_node_t *block)
{
  invariant(program == NULL, "program is NULL");
  invariant(block == NULL, "block is NULL");
  invariant(block->type != JKL_NODE_BLOCK, "the node is not a block type");

  for (unsigned int i = 0; i < block->compound.n_nodes; i++) {
    jkl_node_t *child = block->compound.nodes[i];
    switch (child->type) {
      case JKL_NODE_LET: {
        jkl_string_t name = child->id->value.s;
        jkl_symbol_t *sym = jkl_symbol_table_get(program->symbol_table, name);
        jkl_word_t slot = sym ? sym->slot
                              : jkl_symbol_table_add(program->symbol_table, name,
                                                     JKL_SYMBOL_LET);

        jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_ALLOC, slot, 0, 0));
        jkl_compile_expr(program, child->expr);
        jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_STORE, slot, 0, 0));
        break;
      }
      case JKL_NODE_IF: {
        jkl_compile_expr(program, child->expr);
        jkl_word_t jcp = jkl_ir_code_push(program->ir_code,
                                          JKL_EMIT_IR(JKL_IR_JCP, 0, 0, 0));
        jkl_compile_block(program, child->block);

        if (child->block_else == NULL) {
          jkl_ir_code_patch(program->ir_code, jcp, 0, program->ir_code->n_irs);
        } else {
          jkl_word_t jmp = jkl_ir_code_push(program->ir_code,
                                            JKL_EMIT_IR(JKL_IR_JMP, 0, 0, 0));
          jkl_ir_code_patch(program->ir_code, jcp, 0, program->ir_code->n_irs);
          jkl_compile_block(program, child->block_else);
          jkl_ir_code_patch(program->ir_code, jmp, 0, program->ir_code->n_irs);
        }
        break;
      }
      case JKL_NODE_LOOP: {
        jkl_qqword_t loop_start = program->ir_code->n_irs;
        jkl_compile_block(program, child->block);
        jkl_ir_code_push(program->ir_code,
                         JKL_EMIT_IR(JKL_IR_JMP, loop_start, 0, 0));
        break;
      }
      case JKL_NODE_CALL: {
        jkl_word_t nargs = 0;
        if (child->node != NULL) {
          jkl_compile_expr(program, child->node);
          nargs = 1;
        }
        jkl_word_t idx = jkl_ir_code_push(program->ir_code,
                                          JKL_EMIT_IR(JKL_IR_CALL, 0, 0, nargs));
        jkl_call_fixup_push(program, idx, child->id->value.s);
        break;
      }
      case JKL_NODE_FUNC: {
        jkl_string_t name = child->id->value.s;
        if (jkl_symbol_table_get(program->symbol_table, name) == NULL) {
          jkl_symbol_table_add(program->symbol_table, name, JKL_SYMBOL_FUNCTION);
        }
        jkl_func_queue_push(program, child);
        program->n_funcs++;
        break;
      }
      case JKL_NODE_RETURN: {
        jkl_compile_expr(program, child->expr);
        jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_RET, 0, 0, 0));
        break;
      }
      default:
        jkl_error("jkl_compiler", "does not support this node element yet");
        break;
    }
  }

  return 0;
}

/*
 * Emit a hoisted function body: record its entry address, bind its (single)
 * param, compile the body, and ensure it ends in RET.
 */
static void jkl_compile_func(jkl_program_t *program, jkl_node_t *func)
{
  jkl_string_t name = func->id->value.s;
  jkl_qqword_t entry = program->ir_code->n_irs;

  jkl_symbol_t *sym = jkl_symbol_table_get(program->symbol_table, name);
  if (sym != NULL) {
    sym->addr = entry;
  }

  jkl_node_t *params = func->params;
  jkl_word_t n_params = params ? params->compound.n_nodes : 0;
  if (n_params > 1) {
    jkl_warn("jkl_compiler",
             "function '%s' declares %d params but calls bind only the first",
             name, n_params);
  }

  for (jkl_word_t i = 0; i < n_params; i++) {
    jkl_string_t pname = params->compound.nodes[i]->id->value.s;
    jkl_symbol_t *psym = jkl_symbol_table_get(program->symbol_table, pname);
    jkl_word_t pslot = psym ? psym->slot
                            : jkl_symbol_table_add(program->symbol_table, pname,
                                                   JKL_SYMBOL_LET);
    jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_ALLOC, pslot, 0, 0));
    if (i == 0) {
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_STORE, pslot, 0, 0));
    }
  }

  jkl_compile_block(program, func->block);

  jkl_qqword_t n = program->ir_code->n_irs;
  if (n == entry || program->ir_code->ir[n - 1].type != JKL_IR_RET) {
    jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_RET, 0, 0, 0));
  }
}

/*
 * Resolve deferred CALL targets now that every function entry is known.
 * A name that resolves to a FUNCTION is an internal call (kind 0, target =
 * entry address); anything else is treated as an external/builtin call
 * (kind 1, target = bss offset of the NUL-terminated callee name).
 */
static void jkl_resolve_call_fixups(jkl_program_t *program)
{
  for (size_t i = 0; i < program->n_fixups; i++) {
    jkl_word_t idx = program->call_fixups[i].idx;
    jkl_string_t name = program->call_fixups[i].name;
    jkl_symbol_t *sym = jkl_symbol_table_get(program->symbol_table, name);

    if (sym != NULL && sym->type == JKL_SYMBOL_FUNCTION) {
      jkl_ir_code_patch(program->ir_code, idx, 0, sym->addr);
    } else {
      jkl_word_t off = jkl_bss_intern(program, name);
      jkl_ir_code_patch(program->ir_code, idx, 0, off);
      jkl_ir_code_patch(program->ir_code, idx, 1, 1);
    }
  }
}

jkl_word_t jkl_compile(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  invariant(program->ast_prog_root == NULL, "ast root is NULL");

  jkl_log("jkl_compiler", "compiling program");

  /* phase 1: top-level (funcs enqueue, calls emit placeholders) */
  jkl_compile_block(program, program->ast_prog_root);
  jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_HALT, 0, 0, 0));

  /* phase 2: drain the func worklist (compile_func may enqueue nested funcs) */
  for (size_t i = 0; i < program->n_func_queue; i++) {
    jkl_compile_func(program, program->func_queue[i]);
  }

  /* phase 3: resolve all deferred CALL targets */
  jkl_resolve_call_fixups(program);

  return 0;
}
