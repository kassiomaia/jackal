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
  program->ir_code = malloc(sizeof(jkl_ir_code_t));
  if (program->ir_code == NULL) {
    jkl_error("jkl_compiler", "cannot allocate memory for ir code");
  }

  program->ir_code->ir = malloc(sizeof(jkl_ir_t) * 1024);
  if (program->ir_code->ir == NULL) {
    jkl_error("jkl_compiler", "cannot allocate memory for ir code");
  }

  program->ir_code->n_irs = 0;
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
      jkl_ir_store_string(program->ir_code, expr->value.s);
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_LOAD, hash,
                       hash + jkl_string_len(expr->value.s), 0));
      break;
    }
    case JKL_NODE_ID: {
      jkl_qword_t hash = jkl_string_hash(expr->value.s);
      jkl_ir_store_string(program->ir_code, expr->value.s);
      jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_LOAD, hash, 0, 0));
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
    default: {
      jkl_print_ast_type(expr);
      jkl_error("jkl_compiler", "invalid expression node");
    }
  }

  return 0;
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
        jkl_word_t hash = jkl_string_hash(child->id->value.s);
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_LET");

        jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_ALLOC, hash, 0, 0));
        jkl_compile_expr(program, child->expr);
        jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_STORE, hash, 0, 0));
        break;
      }
      case JKL_NODE_IF: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_IF");

        jkl_word_t ifb_pos = program->ir_code->n_irs - 1;
        jkl_compile_expr(program, child->expr);
        jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_JCP, ifb_pos, 0, 0));
        jkl_compile_block(program, child->block);
        break;
      }
      case JKL_NODE_LOOP: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_LOOP");
        break;
      }
      case JKL_NODE_CALL: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_CALL");
        jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_CALL, 0, 0, 0));
        break;
      }
      case JKL_NODE_FUNC: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_FUNC");
        program->n_funcs++;
        jkl_compile_block(program, child->block);
        break;
      }
      case JKL_NODE_RETURN: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_RETURN");
        break;
      }
      default:
        jkl_print_ast_type(child);
        jkl_error("jkl_compiler", "does not support this node element yet");
        break;
    }
  }
}

jkl_word_t jkl_compile(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  invariant(program->ast_prog_root == NULL, "ast root is NULL");

  jkl_log("jkl_compiler", "compiling program");
  jkl_compile_block(program, program->ast_prog_root);
  jkl_ir_code_push(program->ir_code, JKL_EMIT_IR(JKL_IR_HALT, 0, 0, 0));

  return 0;
}
