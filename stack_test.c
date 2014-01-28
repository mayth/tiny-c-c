#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

typedef struct {
  int x;
  int y;
} Point;

Point *Point_new(const int x, const int y) {
  Point *p = (Point *)malloc(sizeof(Point));
  p->x = x;
  p->y = y;
  return p;
}

void Point_delete(Point *p) {
  if (p)
    free(p);
}

void Point_print(Point *p) {
  printf("(%d, %d)", p->x, p->y);
}

int main(int argc, char const* argv[])
{
  Stack *st = Stack_new();
  StackIterator *iter;
  Stack_push(st, Point_new(1, 3));
  Stack_push(st, Point_new(1, 6));
  Stack_push(st, Point_new(4, 4));
  Stack_push(st, Point_new(5, 8));
  Stack_push(st, Point_new(1, 5));
  Stack_push(st, Point_new(4, 3));

  if (Stack_isEmpty(st)) {
    printf("stack is empty!\n");
  } else {
    printf("stack is not empty!\n");
  }

  iter = Stack_iterator(st);
  while (StackIter_moveNext(iter)) {
    Point *p = (Point *)StackIter_current(iter);
    printf("iter: (%d,%d)\n", p->x, p->y);
  }
  StackIter_delete(iter);

  printf("pop: ");
  Point_print((Point *)Stack_pop(st));
  printf("\n");

  printf("pop: ");
  Point_print((Point *)Stack_pop(st));
  printf("\n");

  while (!Stack_isEmpty(st)) {
    Point *p = (Point *)Stack_pop(st);
    printf("lp_pop: (%d,%d)\n", p->x, p->y);
  }

  if (Stack_isEmpty(st)) {
    printf("stack is empty!\n");
  } else {
    printf("stack is not empty!\n");
  }

  return 0;
}
