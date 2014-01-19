#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "AST.h"
#include "trie.h"

Trie *SymbolTable;
AST *Root;

int AST_comparer(const void *a, const void *b) {
  if (a == b) {
    return 0;
  }
  return -1;
}

AST *AST_alloc() {
  AST *p = (AST *)malloc(sizeof(AST));
  if (!p) {
    fprintf(stderr, "Cannot allocate memory for AST.\n");
    abort();
  }
  return p;
}

AST* AST_makeValue(int v) {
  AST *p;
  p = AST_alloc();
  p->code = VAL_NUM;
  p->AST_value = v;
  return p;
}

AST* AST_makeBinary(CodeType code, AST *left, AST *right) {
  AST *p;
  p = AST_alloc();
  p->code = code;
  p->AST_left = left;
  p->AST_right = right;
  return p;
}

AST *AST_makeUnary(CodeType code, AST *node) {
  AST *p = AST_alloc();
  p->code = code;
  p->AST_left = node;
  p->AST_right = NULL;
  return p;
}

Symbol *AST_lookupSymbol(const char *name) {
  Symbol *p = trie_search_leaf(SymbolTable, name);
  if (!p) {
    p = Symbol_new();
    trie_store(SymbolTable, name, p);
  }
  return p;
}

AST *AST_makeSymbol(const char *name) {
  AST *p = AST_alloc();
  p->code = VAL_SYMBOL;
  p->AST_symbol_name = name;
  p->AST_symbol = AST_lookupSymbol(name);
  return p;
}

AST *AST_makeList(AST *node) {
  AST *p = AST_alloc();
  p->code = ETC_LIST;
  p->AST_list = List_new(AST_comparer);
  return AST_addList(p, node);
}

AST *AST_addList(AST *p, AST *e) {
  assert(p->code == ETC_LIST);
  List_add(p->AST_list, e);
  return p;
}

AST *AST_getList(AST *p, unsigned long index) {
  assert(p->code == ETC_LIST);
  return List_get_at(p->AST_list, index);
}

void AST_free(AST *ast) {
  if (ast != NULL) {
      free(ast);
  }
}

Symbol *Symbol_alloc() {
  Symbol *p = (Symbol *)malloc(sizeof(Symbol));
  if (!p) {
    fprintf(stderr, "Cannot allocate memory for Symbol.\n");
    abort();
  }
  return p;
}

Symbol *Symbol_new() {
  Symbol *p = Symbol_alloc();
  p->SYM_value = 0;
  return p;
}
