#ifndef tinyc_stack_h
#define tinyc_stack_h

#include <stdbool.h>

typedef struct stack_node_t StackNode;
struct stack_node_t {
  StackNode *next;
  void *value;
};

typedef struct {
  StackNode *head;
  unsigned long size;
} Stack;

typedef enum stack_iteration_state_t {
  SIST_UNKNOWN = 0,
  SIST_BEFORE,
  SIST_RUNNING,
  SIST_AFTER
} StackIterationState;

typedef struct {
  Stack *stack;
  StackNode *current;
  StackIterationState state;
} StackIterator;

Stack *Stack_new();
void Stack_delete(Stack *stack);
unsigned long Stack_size(Stack *stack);
bool Stack_isEmpty(Stack *stack);
void Stack_push(Stack *stack, void *value);
void *Stack_pop(Stack *stack);
StackIterator *Stack_iterator(Stack *stack);

StackNode *StackNode_new(void *value);
void StackNode_delete(StackNode *node);

StackIterator *StackIter_new(Stack *stack);
void StackIter_delete(StackIterator *iter);
void *StackIter_current(StackIterator *iter);
bool StackIter_moveNext(StackIterator *iter);

#endif
