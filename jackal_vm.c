#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jackal/jackal_types.h>
#include <jackal/jackal_vm.h>
#include <jackal/jackal_ast.h>
#include <jackal/jackal_dump.h>
#include <jackal/jackal_error.h>
#include <sys/mman.h>
#include <unistd.h>
#include <time.h>

__attribute__((nothrow)) jkl_word_t jkl_vm_run(jkl_vm_t *vm);


jkl_word_t jkl_vm_stack_push(jkl_vm_t *vm, jkl_value_t value)
{
  jkl_log("jkl_vm_stack_push", "Pushing value to stack: %ld", value.v_qword);
  jkl_word_t i = vm->frame->sp;
  vm->frame->values[i] = value;
  vm->frame->sp++;
  jkl_log("jkl_vm_stack_push", "stack pointer: %lld", vm->frame->sp);
  return i;
}

void jkl_vm_print_stack(jkl_vm_t *vm)
{
  for (jkl_word_t i = vm->frame->bp; i < vm->frame->sp; i++)
    {
      jkl_value_t value = vm->frame->values[i];
      if (value.type == JKL_TYPE_NONE)
        jkl_log("jkl_vm_print_stack", "value is none");
      else if (value.type == JKL_TYPE_BOOL)
        jkl_log("jkl_vm_print_stack", "value is bool: %s", value.v_bool ? "true" : "false");
      else if (value.type == JKL_TYPE_BYTE)
        jkl_log("jkl_vm_print_stack", "value is byte: %d", value.v_byte);
      else if (value.type == JKL_TYPE_WORD)
        jkl_log("jkl_vm_print_stack", "value is word: %d", value.v_word);
      else if (value.type == JKL_TYPE_DWORD)
        jkl_log("jkl_vm_print_stack", "value is dword: %d", value.v_dword);
      else if (value.type == JKL_TYPE_QWORD)
        jkl_log("jkl_vm_print_stack", "value is qword: %ld", value.v_qword);
      else if (value.type == JKL_TYPE_QQWORD)
        jkl_log("jkl_vm_print_stack", "value is qqword: %lld", value.v_qqword);
      else if (value.type == JKL_TYPE_FLOAT)
        jkl_log("jkl_vm_print_stack", "value is float: %f", value.v_float);
      else if (value.type == JKL_TYPE_DOUBLE)
        jkl_log("jkl_vm_print_stack", "value is double: %f", value.v_double);
      else if (value.type == JKL_TYPE_STRING)
        jkl_log("jkl_vm_print_stack", "value is string: %s", value.v_string);
    }
}

jkl_word_t jkl_vm_stack_pop(jkl_vm_t *vm, jkl_value_t *value)
{
  if (vm->frame->sp == 0)
    return -1;

  jkl_word_t i = vm->frame->sp - 1;
  *value = vm->frame->values[i];
  vm->frame->sp--;
  return 0;
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
   * Memory
   */

  int page_size = getpagesize();
  int n_pages = JKL_VM_PAGE_OBJECTS_SIZE / page_size;
  jkl_log("jkl_vm_init", "page size: %d", page_size);
  jkl_log("jkl_vm_init", "n_pages: %d", n_pages);

  vm->objects = mmap(NULL, JKL_VM_PAGE_OBJECTS_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (vm->objects == MAP_FAILED)
    jkl_error("jkl_vm_init", "failed to allocate memory for objects");

  memset(vm->objects, 0, JKL_VM_PAGE_OBJECTS_SIZE);
  vm->n_objects = 0;
  vm->n_objects_max = JKL_VM_PAGE_OBJECTS_SIZE / sizeof(jkl_heap_object_t);
  jkl_log("jkl_vm_init", "n_objects_max: %d", vm->n_objects_max);

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

void jkl_vm_reset(jkl_vm_t *vm)
{
  if (vm == NULL)
    jkl_error("jkl_vm_reset", "vm is null");

  vm->pc = 0;
  vm->halted = 0;
  vm->frame->pc = 0;
  vm->frame->bp = 0;
  vm->frame->sp = 0;
  vm->frame->fp = 0;
  free(vm->heap->root);
  free(vm->heap);

  munmap(vm->objects, JKL_VM_PAGE_OBJECTS_SIZE);
  vm->objects = NULL;

  jkl_vm_init(vm);
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

        jkl_heap_object_t *obj = vm->objects[ilabel];
        if (obj == NULL)
          {
            jkl_log("jackal_vm", "heap object not found.");
            goto halt;
          }

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
        vm->objects[ilhs] = rhs_obj;
        break;
      }
    case JKL_LPB:
      break;
    case JKL_RAI:
      {
        jkl_word_t imsg = inst->args[0];
        jkl_string_t msg = vm->program->bss.values[imsg].v_string;
        fprintf(vm->io.err, "\033[0;31m[runtime_error]\033[0m \033[1;31m%s\033[0m\n", msg);
        goto halt;
      }
    case JKL_PTS:
      jkl_log("jackal_vm", "popping value from the stack.");
      jkl_value_t* val;
      jkl_word_t r = jkl_vm_stack_pop(vm, val);
      if (r == -1)
        jkl_error("jackal_vm", "stack underflow.");
      if (val == NULL)
        fprintf(vm->io.err, "%s", "<NULL>\n");

      fprintf(vm->io.out, "%s", val->v_string);
      break;
    case JKL_LPE:
      jkl_word_t pc = inst->args[0];
      vm->pc = pc;
      continue;
    }

    #ifdef CYCLE_RATE
      #ifndef CYCLE_RATE
        #define CYCLE_RATE 1000000
      #endif
      jkl_note("jackal_vm", "sleeping for %d microseconds.", CYCLE_RATE);
      usleep(CYCLE_RATE);
    #endif

    #ifdef ANALYSE_INSTRUCTIONS
      jkl_log("jackal_vm", "instruction %d executed.", inst->type);
      jkl_log("jackal_vm", "pc is now %d.", vm->pc);
      jkl_log("jackal_vm", "stack size is %d.", vm->frame->sp);
analyse:
      jkl_note("jackal_vm", "waiting input key: (q)uit, (p)rint stack, (s)tep, (r)eset.");
      int key = getchar();
      if (key == 'q')
        goto halt;

      if (key == 'p')
        {
          jkl_log("jackal_vm", "printing stack.");
          jkl_vm_print_stack(vm);
          jkl_log("jackal_vm", "stack printed.");
        }

      if (key == 's')
      {
        jkl_log("jackal_vm", "stepping.");
        vm->pc++;
        continue;
      }

      if (key == 'r')
      {
        jkl_log("jackal_vm", "resetting the vm.");
        jkl_vm_reset(vm);
        vm->pc = 0;
        continue;
      }

      goto analyse;
    #endif

    vm->pc++;
  }

halt:
  jkl_log("jackal_vm", "halting the vm.");
  vm->halted = 1;
  vm->pc = 0;
  jkl_word_t r = munmap(vm->io.memory, JKL_VM_PAGE_OBJECTS_SIZE);

  if (r != 0)
    jkl_error("jackal_vm", "failed to unmap the memory.");

  return 0;
}
