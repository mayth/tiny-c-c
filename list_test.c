#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "list.h"

int comparer(const void *a, const void *b) {
  return (int)a - (int)b;
}

bool print_callback(const unsigned long i, const void *v) {
  printf("%lu: %d\n", i, (int)v);
  return true;
}

int main(int argc, char const* argv[])
{
  List *l = List_new(comparer);
  srand(393939);
  for (int i = 0; i < 10; ++i) {
    List_add(l, (void *)((unsigned long)rand()));
  }
  printf("-----\n");
  List_each(l, print_callback);
  List_remove_at(l, 5);
  printf("-----\n");
  List_each(l, print_callback);
  printf("-----\n");
  printf("%d\n", (int)List_get_at(l, 3));
  return 0;
}
