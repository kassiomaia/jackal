#ifndef JACKAL_STACK_H
#define JACKAL_STACK_H

typedef struct {
  void *data;
  unsigned int size;
  enum {
    STACK_IT_CHAR,
    STACK_IT_INT,
    STACK_IT_FLOAT,
    STACK_IT_STRING,
  } type;
} stack_item_t;

typedef struct stack {
  unsigned int bp;
  unsigned int sp;
  unsigned int capacity;
  stack_item_t *items;
} stack_t;

void stack_init(stack_t *s, unsigned int capacity);

void stack_push(stack_t *s, stack_item_t item);

#define stack_pushc(s, c) stack_push(s, (stack_item_t){.data = (void *)(c), .size = sizeof(char), .type = STACK_IT_CHAR})
#define stack_pushi(s, i) stack_push(s, (stack_item_t){.data = (void *)(i), .size = sizeof(int), .type = STACK_IT_INT})
#define stack_pushf(s, f) stack_push(s, (stack_item_t){.data = (void *)(f), .size = sizeof(float), .type = STACK_IT_FLOAT})
#define stack_pushs(s, str) stack_push(s, (stack_item_t){.data = (void *)(str), .size = strlen(str), .type = STACK_IT_STRING})

stack_item_t stack_pop(stack_t *s);

stack_item_t stack_peek(stack_t *s);

unsigned int stack_size(stack_t *s);

void stack_free(stack_t *s);

#endif
