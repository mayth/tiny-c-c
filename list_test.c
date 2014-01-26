#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "list.h"

int comparer(const void *a, const void *b) {
  return (int)a - (int)b;
}

int main(int argc, char const* argv[])
{
  List *l = List_new(comparer);
  ListIterator *iter;
  srand(393939);
  for (int i = 0; i < 10; ++i) {
    List_add(l, (void *)((unsigned long)rand()));
  }
  printf("-----\n");
  iter = List_iterator(l);
  while (ListIter_move_next(iter)) {
    printf("%d\n", (int)ListIter_current(iter));
  }
  ListIter_delete(iter);
  List_remove_at(l, 5);
  printf("-----\n");
  iter = List_iterator(l);
  while (ListIter_move_next(iter)) {
    printf("%d\n", (int)ListIter_current(iter));
  }
  ListIter_delete(iter);
  printf("-----\n");
  printf("%d\n", (int)List_get_at(l, 3));
  return 0;
}
