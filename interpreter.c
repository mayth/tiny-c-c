#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "cstl/vector.h"
#include "stack.h"
#include "AST.h"

typedef struct {
    SymbolType type;
    const char *name;
    union {
        int value;
        struct {
            int *data;
            size_t size;
        } array;
    } un;
} Environment;

Environment *Environment_new_value(Symbol *symbol, int value);
Environment *Environment_new_array(Symbol *symbol, size_t n);
void Environment_delete(Environment *env);

static int return_value = 0;
static Stack *Env = NULL;

int yydebug;
int yyparse();
int executeProgram();
int executeFunction(const AST *body, ASTVector *args, SymbolVector *param);
int callFunction(const AST *ast, ASTVector *args);
int callFunction_(const Symbol *sym, ASTVector *args);
int executeStatements(ASTVector *statements);
bool executeStatement(AST *ast);
int executeExpression(AST *expr);
int executeAssign(AST *ast, AST *expr);
int executeArrayAssign(AST *ast, AST *idx, AST *expr);
int resolveSymbol(const Symbol *symbol, const size_t index);

int main() {
    yydebug = 0;
    SymbolTable = StrSymMap_new();
    Env = Stack_new();
    printf("*** start parsing ***\n");
    int result = yyparse();
    if (result != 0) {
        fprintf(stderr, "!!! Errors on parsing");
        return EXIT_FAILURE;
    }
    printf("*** parsed ***\n");
    printf("*** start execution ***\n");
    result = executeProgram();
    printf("*** finished execution ***\n");
    StrSymMap_delete(SymbolTable);
    SymbolTable = NULL;
    return result;
}

int executeProgram() {
    Symbol *main = AST_lookupSymbol("main");
    if (!main) {
        fprintf(stderr, "Could not find 'main'\n");
        exit(EXIT_FAILURE);
    }
    if (main->type != SYM_FUNC) {
        fprintf(stderr, "'main' is not a function.\n");
        exit(EXIT_FAILURE);
    }
    return callFunction_(main, NULL);
}

// args : List<Expression>
// params : List<Symbol>
int bindArgs(ASTVector *args, SymbolVector *params) {
    unsigned long argc = ASTVector_size(args);
    if (argc != SymbolVector_size(params)) {
        fprintf(stderr, "Argument Error (%lu args given, but expects %lu)\n", argc, SymbolVector_size(params));
        abort();
    }
    for (unsigned long i = 0; i < argc; ++i) {
        AST *expr = ASTVector_at(args, i);
        Symbol *symbol = SymbolVector_at(params, i);
        Environment *e = Environment_new_value(symbol, executeExpression(expr));
        Stack_push(Env, e);
    }
    return argc;
}

void unbindArgs(const int argc) {
    for (int i = 0; i < argc; ++i) {
        Stack_pop(Env);
    }
}

int executeFunction(const AST *body, ASTVector *args, SymbolVector *params) {
    assert(body->code == ETC_LIST);
    bool isExistParam = !SymbolVector_empty(params);
    int argc = 0;
    if (isExistParam) bindArgs(args, params);
    executeStatements(body->AST_list);
    if (isExistParam) unbindArgs(argc);
    return return_value;
}

int callFunction_(const Symbol *sym, ASTVector *args) {
    return executeFunction(sym->SYM_body, args, sym->SYM_param);
}

int callFunction(const AST *ast, ASTVector *args) {
    Symbol *sym = ast->AST_symbol;
    if (sym->type == SYM_UNBOUND) {
        fprintf(stderr, "[func] Used an undefined or uninitialized symbol: %s\n", sym->name);
        abort();
    }
    return callFunction_(sym, args);
}

int referenceArray(AST *ast, AST *expr) {
    Symbol *array = ast->AST_symbol;
    if (array->type == SYM_UNBOUND) {
        fprintf(stderr, "[func] Used an undefined or uninitialized symbol: %s\n", array->name);
        abort();
    }
    size_t idx = executeExpression(expr);
    if (array->SYM_array_size <= idx) {
        fprintf(stderr, "Error: Index out of range.\n");
        exit(EXIT_FAILURE);
    }
    return array->SYM_array_data[idx];
}

int executeExpression(AST *expr) {
    switch (expr->code) {
        case VAL_NUM:
            return expr->AST_value;
        case VAL_SYMBOL:
            return resolveSymbol(expr->AST_symbol, 0);
        case OP_ADD:
            return executeExpression(expr->AST_left) + executeExpression(expr->AST_right);
        case OP_SUB:
            return executeExpression(expr->AST_left) - executeExpression(expr->AST_right);
        case OP_MUL:
            return executeExpression(expr->AST_left) * executeExpression(expr->AST_right);
        case OP_DIV:
            return executeExpression(expr->AST_left) / executeExpression(expr->AST_right);
        case OP_COMPARE_EQ:
            return executeExpression(expr->AST_left) == executeExpression(expr->AST_right);
        case OP_COMPARE_NEQ:
            return executeExpression(expr->AST_left) != executeExpression(expr->AST_right);
        case OP_COMPARE_LT:
            return executeExpression(expr->AST_left) < executeExpression(expr->AST_right);
        case OP_COMPARE_GT:
            return executeExpression(expr->AST_left) > executeExpression(expr->AST_right);
        case OP_COMPARE_LE:
            return executeExpression(expr->AST_left) <= executeExpression(expr->AST_right);
        case OP_COMPARE_GE:
            return executeExpression(expr->AST_left) >= executeExpression(expr->AST_right);
        case OP_ASSIGN:
            return executeAssign(expr->AST_left, expr->AST_right);
        case OP_ASSIGN_ARRAY:
            return executeArrayAssign(expr->AST_first, expr->AST_second, expr->AST_third);
        case OP_CALL:
            return callFunction(expr->AST_left, expr->AST_right->AST_list);
        case OP_REF_ARRAY:
            return referenceArray(expr->AST_left, expr->AST_right);
        default:
            fprintf(stderr, "unknown expression (type: %d)\n", expr->code);
            abort();
    }
}

int assign(Symbol *symbol, const int value) {
    if (symbol->type != SYM_UNBOUND && symbol->type != SYM_VALUE) {
        fprintf(stderr, "Error: Attempt to assign to the unassignable variable %s.\n", symbol->name);
        abort();
    }
    symbol->type = SYM_VALUE;
    symbol->SYM_value = value;
    return value;
}

int arrayAssign(Symbol *symbol, const size_t index, const int value) {
    if (symbol->type != SYM_ARRAY) {
        fprintf(stderr, "Error: Attempt to assign to non-array variable %s.\n", symbol->name);
        exit(EXIT_FAILURE);
    }
    if (symbol->SYM_array_size <= index) {
        fprintf(stderr, "Error: Index out of range.\n");
        exit(EXIT_FAILURE);
    }
    symbol->SYM_array_data[index] = value;
    return value;
}

int executeAssign(AST *sym, AST *expr) {
    return assign(sym->AST_symbol, executeExpression(expr));
}

int executeArrayAssign(AST *sym, AST *idx, AST *expr) {
    return arrayAssign(sym->AST_symbol, executeExpression(idx), executeExpression(expr));
}

void executePrint(AST *ast) {
    printf("%d\n", executeExpression(ast->AST_unary));
}

int executeStatements(ASTVector *statements) {
    const size_t n = ASTVector_size(statements);
    for (size_t i = 0; i < n; ++i) {
        if (!executeStatement(ASTVector_at(statements, i)))
            break;
    }
    return return_value;
}

void executeFor(AST *ast) {
    ASTVector *for_stmt = ast->AST_left->AST_list;
    ASTVector *main = ast->AST_right->AST_list;
    AST *init = ASTVector_at(for_stmt, 0);
    AST *cond = ASTVector_at(for_stmt, 1);
    AST *update = ASTVector_at(for_stmt, 2);
    for (executeExpression(init);
         executeExpression(cond);
         executeExpression(update)) {
        executeStatements(main);
    }
}

bool executeStatement(AST *ast) {
    switch (ast->code) {
        case ETC_LIST:
            executeStatements(ast->AST_list);
            break;
        case CODE_PRINT:
            executePrint(ast);
            break;
        case CODE_RETURN:
            if (ast->AST_unary) {
                return_value = executeExpression(ast->AST_unary);
            }
            return false;
        case CODE_FOR:
            executeFor(ast);
            break;
        case OP_ASSIGN:
        case OP_ASSIGN_ARRAY:
        case OP_CALL:
            executeExpression(ast);
            break;
        case OP_REF_ARRAY:
        case CODE_VAR:
        case VAL_NUM:
        case VAL_SYMBOL:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
            break;
        default:
            fprintf(stderr, "Unknown statement (type: %d)\n", ast->code);
            abort();
    }
    return true;
}

int resolveSymbol(const Symbol *symbol, const size_t index) {
    // find from environment
    StackIterator *iter = Stack_iterator(Env);
    while (StackIter_moveNext(iter)) {
        Environment *e = (Environment *)StackIter_current(iter);
        if (e->name == symbol->name) {
            StackIter_delete(iter);
            switch (e->type) {
                case SYM_VALUE:
                    return e->un.value;
                case SYM_ARRAY:
                    return e->un.array.data[index];
                default:
                    fprintf(stderr, "Error: symbol '%s' found but it has unexpected type (%d).\n", e->name, e->type);
                    exit(EXIT_FAILURE);
            }
        }
    }
    StackIter_delete(iter);
    
    // from symbol table
    if (symbol->type == SYM_UNBOUND) {
        fprintf(stderr, "[expr] Used undefined or uninitialized symbol: %s\n", symbol->name);
        abort();
    }
    return symbol->SYM_value;
}

void AST_initializeVariable(AST *symbol_ast, AST *expr) {
    Symbol *symbol = symbol_ast->AST_symbol;
    if (expr) {
        assign(symbol, executeExpression(expr));
    } else {
    }
}

void AST_declareArray(AST *name, AST *expr) {
    Symbol *symbol = name->AST_symbol;
    int num_elements = executeExpression(expr);
    if (num_elements < 0) {
        fprintf(stderr, "The number of elements for array '%s' must be greater than 0.\n", symbol->name);
        exit(EXIT_FAILURE);
    }
    symbol->type = SYM_ARRAY;
    int *arr = (int *)calloc(num_elements, sizeof(int));
    if (!arr) {
        fprintf(stderr, "Cannot allocate memory for array (requested %d elements)\n", num_elements);
        exit(EXIT_FAILURE);
    }
    symbol->SYM_array_data = arr;
    symbol->SYM_array_size = num_elements;
}

Environment *Environment_new_value(Symbol *symbol, int value) {
    Environment *env = (Environment *)malloc(sizeof(Environment));
    if (!env) {
        fprintf(stderr, "Cannot allocate memory for Environment.\n");
        exit(EXIT_FAILURE);
    }
    env->name = symbol->name;
    env->type = SYM_VALUE;
    env->un.value = value;
    return env;
}

Environment *Environment_new_array(Symbol *symbol, const size_t n) {
    Environment *env = (Environment *)malloc(sizeof(Environment));
    if (!env) {
        fprintf(stderr, "Cannot allocate memory for Environment.\n");
        exit(EXIT_FAILURE);
    }
    env->name = symbol->name;
    env->type = SYM_ARRAY;
    int *arr = (int *)calloc(n, sizeof(int));
    if (!arr) {
        fprintf(stderr, "Cannot allocate memory for Environment (array).\n");
        exit(EXIT_FAILURE);
    }
    env->un.array.data = arr;
    env->un.array.size = n;
    return env;
}

void Environment_delete(Environment *env) {
    if (env) {
        switch(env->type) {
            case SYM_VALUE:
                env->un.value = 0;
                break;
            case SYM_ARRAY:
                free(env->un.array.data);
                env->un.array.data = NULL;
                env->un.array.size = -1;
                break;
            default:
                break;
        }
        free(env);
    }
}

/* vim: set et ts=4 sts=4 sw=4: */
