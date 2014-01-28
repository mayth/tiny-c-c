#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "cstl/unordered_map.h"
#include "cstl/vector.h"
#include "AST.h"

CSTL_VECTOR_IMPLEMENT(ASTVector, AST)
CSTL_VECTOR_IMPLEMENT(SymbolVector, Symbol)
CSTL_UNORDERED_MAP_IMPLEMENT(StrSymMap, const char *, Symbol, StrSymMap_hash_string, strcmp)

StrSymMap *SymbolTable;

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

Symbol *AST_lookupSymbol(char *name) {
  StrSymMapIterator it = StrSymMap_find(SymbolTable, name);
  // If the symbol not found
  if (StrSymMap_end(SymbolTable) == it) {
    it = StrSymMap_insert_ref(SymbolTable, name, Symbol_new(name), NULL);
  }
  return StrSymMap_value(it);
}

AST *AST_wrapSymbol(Symbol *symbol) {
  AST *p = AST_alloc();
  p->code = VAL_SYMBOL;
  p->AST_symbol = symbol;
  return p;
}

AST *AST_makeSymbol(char *name) {
  return AST_wrapSymbol(AST_lookupSymbol(name));
}

SymbolVector * AST_to_symbol_vector(ASTVector *ast_vec) {
  const size_t n = ASTVector_size(ast_vec);
  SymbolVector *v = SymbolVector_new_reserve(n);
  for (size_t i = 0; i < n; ++i) {
    SymbolVector_push_back_ref(v, ASTVector_at(ast_vec, i)->AST_symbol);
  }
  return v;
}

AST *AST_makeFunction(AST *name, AST *params, AST *body) {
  assert(name->code == VAL_SYMBOL);
  assert(params->code == ETC_LIST);
  Symbol *sym = AST_lookupSymbol(name->AST_symbol->name);
  sym->type = SYM_FUNC;
  sym->SYM_param = AST_to_symbol_vector(params->AST_list);
  sym->SYM_body  = body;
  return name;
}

AST *AST_defineVariables(AST *vars) {
  return vars;
}

AST *AST_makeList(AST *node) {
  AST *p = AST_alloc();
  p->code = ETC_LIST;
  p->AST_list = ASTVector_new();
  if (node) {
    AST_addList(p, node);
  }
  return p;
}

AST *AST_addList(AST *p, AST *e) {
  assert(p->code == ETC_LIST);
  ASTVector_push_back_ref(p->AST_list, e);
  return p;
}

AST *AST_getList(AST *p, unsigned long index) {
  assert(p->code == ETC_LIST);
  return ASTVector_at(p->AST_list, index);
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

Symbol *Symbol_new(char *name) {
  Symbol *p = Symbol_alloc();
  p->type = SYM_UNBOUND;
  p->name = name;
  return p;
}
