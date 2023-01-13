#include <jackal.h>

#define invariant(x, msg) \
  if (x) \
    jkl_error("jkl_compiler", "invariant failed: " msg);


FILE *ir;
jkl_ir_code_t ir_code;

void __attribute__((constructor)) jkl_open_ir()
{
  ir = fopen("ir.code", "w");
  if (ir == NULL) {
    jkl_error("jkl_compiler", "cannot create ir file");
  }

  fprintf(ir, "/*\n");
  fprintf(ir, " * IR Code Representation\n");
  fprintf(ir, " * v0.0.1\n");
  fprintf(ir, " */\n");
  fprintf(ir, "\n");

  jkl_ir_code_init(&ir_code, 1024);
}

void __attribute__((destructor)) jkl_close_ir()
{
  fprintf(ir, "/* end of ir */\n");
  fclose(ir);

  jkl_ir_code_save(&ir_code, "ir.bin");
  jkl_ir_code_free(&ir_code);
}

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
  jkl_hash_init(&program->symbol_table);
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
      fprintf(ir, "eql\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_EQL, 0, 0, 0));
      break;
    }
    case JKL_OP_NEQ: {
      fprintf(ir, "neq\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_NEQ, 0, 0, 0));
      break;
    }
    case JKL_OP_GT: {
      fprintf(ir, "gt\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_GT, 0, 0, 0));
      break;
    }
    case JKL_OP_LT: {
      fprintf(ir, "lt\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_LT, 0, 0, 0));
      break;
    }
    case JKL_OP_GTE: {
      fprintf(ir, "gte\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_GTE, 0, 0, 0));
      break;
    }
    case JKL_OP_LTE: {
      fprintf(ir, "lte\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_LTE, 0, 0, 0));
      break;
    }
    case JKL_OP_PLUS: {
      fprintf(ir, "add\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_ADD, 0, 0, 0));
      break;
    }
    case JKL_OP_MINUS: {
      fprintf(ir, "sub\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_SUB, 0, 0, 0));
      break;
    }
    case JKL_OP_MUL: {
      fprintf(ir, "mul\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_MUL, 0, 0, 0));
      break;
    }
    case JKL_OP_DIV: {
      fprintf(ir, "div\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_DIV, 0, 0, 0));
      break;
    }
    case JKL_OP_MOD: {
      fprintf(ir, "mod\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_MOD, 0, 0, 0));
      break;
    }
    case JKL_OP_AND: {
      fprintf(ir, "and\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_AND, 0, 0, 0));
      break;
    }
    case JKL_OP_OR: {
      fprintf(ir, "or\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_OR, 0, 0, 0));
      break;
    }
    case JKL_OP_NOT: {
      fprintf(ir, "not\n");
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_NOT, 0, 0, 0));
      break;
    }
    default:
      jkl_error("jkl_compiler", "invlaid expr op");
  }
}

jkl_word_t jkl_compile_expr(jkl_program_t *program, jkl_node_t *expr)
{
  invariant(program == NULL, "program is NULL");
  invariant(expr == NULL, "block is NULL");

  switch (expr->type) {
    case JKL_NODE_INT: {
      fprintf(ir, "pushi %d\n", expr->value.i);
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_PUSHI, expr->value.i, 0, 0));
      break;
    }
    case JKL_NODE_FLOAT: {
      fprintf(ir, "pushf %f\n", expr->value.f);
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_PUSHF, expr->value.f, 0, 0));
      break;
    }
    case JKL_NODE_STRING: {
      jkl_word_t hash = jkl_string_hash(expr->value.s);
      fprintf(ir, "load [0x%00x:0x%00x]\n", hash,
              hash + jkl_string_len(expr->value.s));
      jkl_ir_store_string(&ir_code, expr->value.s);
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_LOAD, hash,
                                             hash + jkl_string_len(expr->value.s), 0));
      break;
    }
    case JKL_NODE_ID: {
      jkl_qword_t hash = jkl_string_hash(expr->value.s);
      fprintf(ir, "load [ref] 0x%00x\n", hash);
      jkl_ir_store_string(&ir_code, expr->value.s);
      jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_LOAD, hash, 0, 0));
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

        jkl_hash_tbl *props = jkl_hash_new();

        jkl_hash_set(props, "name", "main", JKL_HASH_TYPE_STRING);
        jkl_hash_set(props, "type", "function", JKL_HASH_TYPE_STRING);
        jkl_hash_set(props, "value", "main", JKL_HASH_TYPE_STRING);

        jkl_hash_set(program->symbol_table, child->id->value.s, props,
                     JKL_HASH_TYPE_OBJECT);

        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_LET");
        fprintf(ir, "; let %s = $1\n", child->id->value.s);
        fprintf(ir, "alloc 0x%00x\n", hash);

        jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_ALLOC, hash, 0, 0));
        jkl_compile_expr(program, child->expr);
        jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_STORE, hash, 0, 0));

        fprintf(ir, "store 0x%00x\n", hash);
        break;
      }
      case JKL_NODE_IF: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_IF");
        fprintf(ir, "; if (exp) {\n");
        jkl_word_t ifb_pos = ir_code.n_irs - 1;
        fprintf(ir, "ifb_%d:\n", ifb_pos);

        fprintf(ir, "; begin expr\n");
        jkl_compile_expr(program, child->expr);
        fprintf(ir, "; end expr\n");
        jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_JCP, ifb_pos, 0, 0));
        fprintf(ir, "jcp ife_%d\n", ifb_pos);
        jkl_compile_block(program, child->block);
        fprintf(ir, "ife_%d:\n", ifb_pos);
        fprintf(ir, "; }\n");
        break;
      }
      case JKL_NODE_LOOP: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_LOOP");
        fprintf(ir, "; loop {\n");
        fprintf(ir, "loop:\n");
        fprintf(ir, "goto loop\n");
        fprintf(ir, "endloop:\n");
        fprintf(ir, "; }\n");
        break;
      }
      case JKL_NODE_CALL: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_CALL");
        fprintf(ir, "call $1\n");
        jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_CALL, 0, 0, 0));
        break;
      }
      case JKL_NODE_FUNC: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_FUNC");
        fprintf(ir, "func_%d:\n", program->n_funcs + 1);
        program->n_funcs++;

        jkl_hash_tbl *props = jkl_hash_new();

        jkl_hash_set(props, "name", child->id->value.s, JKL_HASH_TYPE_STRING);
        jkl_hash_set(props, "args", "10", JKL_HASH_TYPE_STRING);

        jkl_hash_set(program->symbol_table, child->id->value.s, props,
                     JKL_HASH_TYPE_OBJECT);
        jkl_compile_block(program, child->block);
        break;
      }
      case JKL_NODE_RETURN: {
        jkl_warn("jkl_compiler", "no rules implemented for JKL_NODE_RETURN");
        fprintf(ir, "return $1\n");
        break;
      }
      default:
        jkl_print_ast_type(child);
        jkl_error("jkl_compiler", "does not support this node element yet");
        break;
    }

    fprintf(ir, "\n");
  }

}

jkl_word_t jkl_compile(jkl_program_t *program)
{
  invariant(program == NULL, "program is NULL");
  invariant(program->ast_prog_root == NULL, "ast root is NULL");

  jkl_hash_tbl *props = jkl_hash_new();

  jkl_hash_set(props, "name", "main", JKL_HASH_TYPE_STRING);
  jkl_hash_set(props, "type", "function", JKL_HASH_TYPE_STRING);
  jkl_hash_set(props, "value", "main", JKL_HASH_TYPE_STRING);

  jkl_hash_set(program->symbol_table, "entry_point", props,
               JKL_HASH_TYPE_OBJECT);

  jkl_log("jkl_compiler", "compiling program");
  jkl_compile_block(program, program->ast_prog_root);
  jkl_ir_code_push(&ir_code, JKL_EMIT_IR(JKL_IR_HALT, 0, 0, 0));
  char *json = malloc(sizeof(char) * 4096);
  memset(json, 0, sizeof(char) * 4096);
  jkl_hash_to_json(program->symbol_table, 0, json);
  jkl_log("jkl_compiler", "symbol table: %s", json);
  free(json);

  jkl_hash_free(program->symbol_table);

  return 0;
}
