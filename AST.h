#ifndef ylaciexpr_AST_h
#define ylaciexpr_AST_h

#define MAX_SYMBOL_LEN (128)

typedef struct AST_ AST;
typedef struct symbol_ Symbol;
typedef enum code_ CodeType;

#include "cstl/vector.h"
#include "cstl/unordered_map.h"

CSTL_VECTOR_INTERFACE(ASTVector, AST)
CSTL_VECTOR_INTERFACE(SymbolVector, Symbol)
CSTL_UNORDERED_MAP_INTERFACE(StrSymMap, const char *, Symbol)

enum code_ {
    ETC_LIST,
    VAL_NUM,
    VAL_SYMBOL,
    CODE_PRINT,
    CODE_FUNC,
    CODE_VAR,
    CODE_RETURN,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_ASSIGN,
    OP_ASSIGN_ARRAY,
    OP_CALL,
    OP_REF_ARRAY
};

typedef enum symbol_type {
    SYM_UNBOUND,
    SYM_VALUE,
    SYM_FUNC,
    SYM_ARRAY
} SymbolType;

struct symbol_ {
    SymbolType type;
    char *name;
    union {
        int value;
        struct {
            int *data;
            size_t size;
        } array;
        struct {
            SymbolVector *params;
            AST *body;
        } func;
    } un;
};

#define SYM_value un.value
#define SYM_param un.func.params
#define SYM_body  un.func.body
#define SYM_array_data un.array.data
#define SYM_array_size un.array.size

struct AST_ {
    CodeType code;
    union {
        /* For value (leaf node) */
        int value;

        /* For symbol */
        Symbol *symbol;

        /* For list */
        ASTVector *list;
        
        AST *unary;

        struct {
            AST *left;
            AST *right;
        } binary;

        struct {
            AST *first;
            AST *second;
            AST *third;
        } trinary;
    } un;
};

#define AST_value un.value
/* For Unary Expression */
#define AST_unary un.unary
/* For Binary Expression */
#define AST_left un.binary.left
#define AST_right un.binary.right
/* For Trinary Expression */
#define AST_first un.trinary.first
#define AST_second un.trinary.second
#define AST_third un.trinary.third
/* For List */
#define AST_list un.list
/* For Symbol Expression */
#define AST_symbol un.symbol

/* Normal Expression */
AST *AST_makeValue(int v);
AST *AST_makeUnary(CodeType type, AST *node);
AST *AST_makeBinary(CodeType type, AST *left, AST *right);
AST *AST_makeTrinary(CodeType type, AST *first, AST *second, AST *third);

/* Symbol Definition */
AST *AST_makeSymbol(char *name);
Symbol *AST_lookupSymbol(char *name);
/* Declarations */
AST *AST_makeFunction(AST *name, AST *params, AST *body);
AST *AST_defineVariables(AST *name);
AST *AST_defineVariable(AST *name, AST *expr);

/* List Operation */
AST *AST_makeList(AST *node);
AST *AST_addList(AST *p, AST *e);
AST *AST_getList(AST *p, unsigned long index);


Symbol *Symbol_new(char *name);

extern StrSymMap *SymbolTable;

/* called from parser */
void AST_declareVariable(AST *name, AST *expr);
void AST_declareArray(AST *name, AST *expr);

#endif

/* vim: set et ts=4 sts=4 sw=4: */
