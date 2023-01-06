#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jackal/jackal_types.h>
#include <jackal/jackal_vm.h>
#include <jackal/jackal_ast.h>
#include <jackal/jackal_dump.h>
#include <jackal/jackal_error.h>

jkl_word_t jkl_vm_stack_push(jkl_vm_t *vm, jkl_value_t value)
{
  jkl_log("jkl_vm_stack_push", "Pushing value to stack: %ld", value.v_qword);
  jkl_word_t i = vm->frame->sp;
  vm->frame->values[i] = value;
  vm->frame->sp++;
  jkl_log("jkl_vm_stack_push", "stack pointer: %lld", vm->frame->sp);
  return i;
}

jkl_value_t* jkl_vm_stack_pop(jkl_vm_t *vm, jkl_value_t *value)
{
  jkl_word_t i = vm->frame->sp - 1;
  value = &vm->frame->values[i];
  vm->frame->sp--;
  return value;
}

jkl_word_t jkl_vm_init(jkl_vm_t *vm)
{
  /*
   * Program Counter
   */
  vm->pc = 0;
  vm->halted = 0;

  /*
   * IO
   */

  vm->io.in = stdin;
  vm->io.out = stdout;
  vm->io.err = stderr;

  /*
   * Stack
   *  - frame: It's a stack of stack frames.
   */

  vm->frame = (jkl_stack_frame_t *)malloc(sizeof(jkl_stack_frame_t));
  vm->frame->pc = 0;
  vm->frame->bp = 0;
  vm->frame->sp = 0;
  vm->frame->fp = 0;
  vm->frame->size = JKL_VM_STACK_FRAME_SIZE;

  /*
   * Heap
   *  - root: It's a special object that is always present in the heap.
   */

  vm->heap = (jkl_heap_t *)malloc(sizeof(jkl_heap_t));
  vm->heap->root = (jkl_heap_object_t *)calloc(1, sizeof(jkl_heap_object_t));
  vm->heap->root->refs.n_refs = 0;
  vm->heap->root->data.size = 0;
  vm->heap->root->data.data = NULL;
  vm->heap->root->opts.is_tagged = 0;
  vm->heap->root->opts.created_at = 0;
  vm->heap->root->opts.updated_at = 0;
  vm->heap->n_objects = 1;

  return 0;
}

jkl_word_t jkl_vm_load(jkl_vm_t *vm, jkl_program_t *program)
{
  if (vm == NULL || program == NULL)
    return 1;

  vm->program = program;
  return 0;
}

jkl_word_t jkl_vm_run(jkl_vm_t *vm)
{
  if (vm == NULL)
    return 1;

  if (vm->program == NULL)
    return 2;

  while (vm->halted == 0)
  {
    jkl_inst_t *inst = &vm->program->code.inst[vm->pc];



    switch (inst->type)
    {
    case JKL_NOP:
      jkl_note("jackal_vm", "the program stopped abruptly.");
      goto halt;
    case JKL_PSH:
      break;
    case JKL_CST:
      {
        jkl_word_t ilabel = inst->args[0];

        jkl_log("jackal_vm", "pushing constant %d to the stack.", ilabel);
        jkl_heap_object_t *obj = jkl_get_symbol_ref(vm->program, ilabel);
        if (obj == NULL)
          {
            jkl_log("jackal_vm", "heap object not found.");
            vm->halted = 1;
            break;
          }

        jkl_dump_value(obj->value);

        if (obj->value == NULL)
            jkl_error("jackal_vm", "value not found.");

        switch (obj->value->type)
        {
        case JKL_TYPE_NONE:
          jkl_error("jackal_vm", "invalid constant type.");
        case JKL_TYPE_STRING:
          {
            jkl_log("jackal_vm", "pushing string to the stack.");
            if (obj->value->type == JKL_TYPE_STRING)
            {
              jkl_word_t i = jkl_vm_stack_push(vm, *obj->value);
              jkl_log("jackal_vm", "pushed string to the stack.");
            }
            jkl_log("jackal_vm", "what string variable from heap: %s", obj->value->v_string);
          }
        default:
          break;
        }
        break;
      }
    case JKL_STR:
      {
        jkl_word_t ilhs = inst->args[0];
        jkl_word_t irhs = inst->args[1];

        jkl_string_t symbol;
        if (jkl_get_symbol(vm->program, ilhs, &symbol))
            jkl_error("jackal_vm", "The symbol with hash '%d' is not defined.", ilhs);

        jkl_log("jackal_vm", "the symbol '%s' '%d' is defined.", symbol, ilhs);

        // load irhs from bss
        jkl_log("jackal_vm", "looking for the symbol '%d' in bss.", irhs);
        jkl_value_t irhs_val = vm->program->bss.values[irhs];
        jkl_log("jackal_vm", "the irhs value is '%p'.", &irhs_val);

        if (irhs_val.type == JKL_TYPE_NONE)
            jkl_error("jackal_vm", " the irhs type is a node.");

        if (irhs_val.type == JKL_TYPE_STRING)
          {
            jkl_log("jackal_vm", "the irhs type is a string.");
            jkl_log("jackal_vm", "the irhs value is '%s'.", irhs_val.v_string);
          }

        // allocate a new object in the heap
        jkl_log("jackal_vm", "allocating memory for the symbol '%d'.", ilhs);
        jkl_heap_object_t *rhs_obj = malloc(sizeof(jkl_heap_object_t));
        rhs_obj->refs.n_refs = 0;

        if (irhs_val.type == JKL_TYPE_STRING)
          {
            jkl_log("jackal_vm", "copy the string to the heap.");
            rhs_obj->value = malloc(sizeof(jkl_value_t));
            rhs_obj->value->type = JKL_TYPE_STRING;
            rhs_obj->value->v_string = (jkl_string_t)malloc(strlen(irhs_val.v_string) + 1);
            strcpy(rhs_obj->value->v_string, irhs_val.v_string);
            jkl_log("jackal_vm", "the string is '%s'.", rhs_obj->value->v_string);
          }
        jkl_log("jackal_vm", "the heap object has been allocated at '%p'.", rhs_obj);

        // add the object to symbol table
        jkl_log("jackal_vm", "set the heap object as reference for the symbol '%d'.", ilhs);
        jkl_bool_t r = jkl_set_symbol(vm->program, ilhs, rhs_obj);
        if (r == 0)
          jkl_log("jackal_vm", "the symbol '%d' has been set.", ilhs)
        else
          jkl_error("jackal_vm", "the symbol '%d' has not been set.", ilhs);

        break;
      }
    case JKL_LPB:
      break;
    case JKL_RAI:
      {
        jkl_word_t imsg = inst->args[0];
        jkl_string_t msg = vm->program->bss.values[imsg].v_string;
        fprintf(vm->io.err, "\033[0;31m[runtime_error]\033[0m \033[1;31m%s\033[0m\n", msg);
        vm->halted = 1;
        break;
      }
    case JKL_PTS:
      fprintf(vm->io.out, "%s", "<null>");
      break;
    case JKL_LPE:
      jkl_word_t pc = inst->args[0];
      vm->pc = pc;
      continue;
    }

    vm->pc++;
  }
}
