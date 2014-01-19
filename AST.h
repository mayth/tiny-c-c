#ifndef ylaciexpr_AST_h
#define ylaciexpr_AST_h

#define MAX_SYMBOL_LEN (128)

#include "trie.h"
#include "list.h"

typedef struct AST_ AST;
typedef enum code_ CodeType;

enum code_ {
    ETC_LIST,
    VAL_NUM,
    VAL_SYMBOL,
    CODE_ASSIGN,
    CODE_PRINT,
    CODE_FUNC,
    CODE_VAR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

typedef enum symbol_type {
    SYM_VALUE,
    SYM_FUNC
} SymbolType;

typedef struct symbol_ {
    SymbolType type;
    union {
        int value;
        struct {
            List *params;
            AST *body;
        } func;
    } un;
} Symbol;

#define SYM_value un.value
#define SYM_param un.func.params
#define SYM_body  un.func.body

struct AST_ {
    CodeType code;
    union {
        /* For value (leaf node) */
        int value;

        /* For symbol */
        struct {
            const char *name;
            Symbol *symbol;
        } symbol;

        /* For list */
        List *list;
        
        /* For others (inner node) */
        struct {
            AST *left;
            AST *right;
        } inner;
    } un;
};


/* Normal Expression */
AST *AST_makeValue(int v);
AST *AST_makeBinary(CodeType type, AST *left, AST *right);
AST *AST_makeUnary(CodeType type, AST *node);

/* Symbol Definition */
AST *AST_makeSymbol(const char *name);
Symbol *AST_lookupSymbol(const char *name);
/* Declarations */
AST *AST_makeFunction(AST *name, AST *params, AST *body);
AST *AST_makeVariable(AST *name, AST *value);

/* List Operation */
AST *AST_makeList(AST *node);
AST *AST_addList(AST *p, AST *e);
AST *AST_getList(AST *p, unsigned long index);


Symbol *Symbol_new();

extern Trie *SymbolTable;
extern AST *Root;

#define AST_value un.value
/* For Unary Expression */
#define AST_unary un.inner.left
/* For Binary Expression */
#define AST_left un.inner.left
#define AST_right un.inner.right
#define AST_list un.list
/* For Symbol Expression */
#define AST_symbol_name un.symbol.name
#define AST_symbol un.symbol.symbol

#endif

/* vim: set et ts=4 sts=4 sw=4: */
