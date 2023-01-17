#include <jackal.h>

void stack_init(stack_t *stack, unsigned int capacity)
{
  stack->capacity = capacity;
  stack->bp = 0;
  stack->sp = 0;
  stack->items = malloc(sizeof(stack_item_t) * capacity);

  if (stack->items == NULL) {
    jkl_error("jkl_stack", "stack_init: malloc failed");
  }

  memset(stack->items, 0, sizeof(stack_item_t) * capacity);
}

void stack_push(stack_t *stack, stack_item_t item)
{
  if (stack->sp == stack->capacity) {
    jkl_error("jkl_stack", "stack_push: stack overflow");
  }

  stack->items[stack->sp++] = item;
}

stack_item_t stack_pop(stack_t *stack)
{
  if (stack->sp == stack->bp) {
    jkl_error("jkl_stack", "stack_pop: stack underflow");
  }

  return stack->items[--stack->sp];
}

stack_item_t stack_peek(stack_t *stack)
{
  if (stack->sp == stack->bp) {
    jkl_error("jkl_stack", "stack_peek: stack underflow");
  }

  return stack->items[stack->sp - 1];
}

unsigned int stack_size(stack_t *stack)
{
  return stack->sp - stack->bp;
}

void stack_free(stack_t *stack)
{
  free(stack->items);
}
