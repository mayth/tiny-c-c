#include <stdio.h>
#include "trie.h"

void traverse(const VALUE v, const char *key) {
  printf("%s: %d\n", key, (int)v);
}

int main(int argc, char const* argv[])
{
  Trie *trie = trie_new();
  trie_store(trie, "abcd", (void *)9);
  trie_store(trie, "abc", (void *)1);
  trie_store(trie, "efg", (void *)31);
  trie_store(trie, "efgh", (void *)100);
  trie_store(trie, "x", (void *)7);
  trie_store(trie, "y", (void *)2);
  trie_store(trie, "z", (void *)3);
  printf("Traverse!\n");
  trie_traverse_with_key(trie, "", traverse);
  VALUE v = trie_delete(trie, "abc");
  printf("deleted abc (value = %d)\n", (int)v);
  printf("Traverse!\n");
  trie_traverse_with_key(trie, "", traverse);
  return 0;
}
