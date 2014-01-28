#include <stdlib.h>
#include <assert.h>
#include "stack.h"

static Stack *Stack_alloc() {
  return (Stack *)malloc(sizeof(Stack));
}

Stack *Stack_new() {
  Stack *p = Stack_alloc();
  if (!p) {
    return NULL;
  }
  p->head = NULL;
  p->size = 0;
  return p;
}

void Stack_delete(Stack *stack) {
  if (stack) {
    stack->head = NULL;
    stack->size = -1;
    free(stack);
  }
}

inline unsigned long Stack_size(Stack *stack) {
  assert(stack != NULL);
  return stack->size;
}

inline bool Stack_isEmpty(Stack *stack) {
  assert(stack != NULL);
  return stack->size == 0;
}

void Stack_push(Stack *stack, void *value) {
  assert(stack != NULL);
  StackNode *node = StackNode_new(value);
  node->next = stack->head;
  stack->head = node;
  ++stack->size;
}

void *Stack_pop(Stack *stack) {
  assert(stack != NULL);
  assert(Stack_size(stack) > 0);
  StackNode *p = stack->head;
  stack->head = p->next;
  --stack->size;
  return p->value;
}

StackIterator *Stack_iterator(Stack *stack) {
  assert(stack != NULL);
  return StackIter_new(stack);
}

static StackNode *StackNode_alloc() {
  return (StackNode *)malloc(sizeof(StackNode));
}

StackNode *StackNode_new(void *value) {
  StackNode *node = StackNode_alloc();
  if (!node) {
    return NULL;
  }
  node->next = NULL;
  node->value = value;
  return node;
}

void StackNode_delete(StackNode *node) {
  if (node) {
    node->next = NULL;
    node->value = NULL;
    free(node);
  }
}

static StackIterator *StackIter_alloc() {
  return (StackIterator *)malloc(sizeof(StackIterator));
}

StackIterator *StackIter_new(Stack *stack) {
  assert(stack != NULL);
  StackIterator *iter = StackIter_alloc();
  if (!iter) {
    return NULL;
  }
  iter->stack = stack;
  iter->current = NULL;
  iter->state = SIST_BEFORE;
  return iter;
}

void StackIter_delete(StackIterator *iter) {
  if (iter) {
    iter->stack = NULL;
    iter->current = NULL;
    iter->state = SIST_UNKNOWN;
    free(iter);
  }
}

void *StackIter_current(StackIterator *iter) {
  assert(iter != NULL);
  if (iter->state == SIST_RUNNING) {
    return iter->current->value;
  } else {
    return NULL;
  }
}

bool StackIter_moveNext(StackIterator *iter) {
    assert(iter != NULL);
    switch (iter->state) {
        case SIST_BEFORE:
            if (iter->stack->head) {
                iter->current = iter->stack->head;
                iter->state = SIST_RUNNING;
                return true;
            } else {
                iter->state = SIST_AFTER;
                return false;
            }
        case SIST_RUNNING:
            if (iter->current->next) {
                iter->current = iter->current->next;
                return true;
            } else {
                iter->current = NULL;
                iter->state = SIST_AFTER;
                return false;
            }
        case SIST_AFTER:
            return false;
        default:
            abort();    // invalid state
    }
}
