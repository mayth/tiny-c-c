#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "cstl/vector.h"
#include "stack.h"
#include "AST.h"

typedef struct {
    Symbol *symbol;
    int value;
} Environment;

Environment *Environment_new(Symbol *symbol, int value);
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
int resolveSymbol(const Symbol *symbol);
int bindSymbol(Symbol *symbol, const int value);

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
        Environment *e = Environment_new(symbol, executeExpression(expr));
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
        fprintf(stderr, "[func] Used an undefined or uninitialized symbol: %s\n", ast->AST_symbol->name);
        abort();
    }
    return callFunction_(sym, args);
}

int executeExpression(AST *expr) {
    switch (expr->code) {
        case VAL_NUM:
            return expr->AST_value;
        case VAL_SYMBOL:
            return resolveSymbol(expr->AST_symbol);
        case OP_ADD:
            return executeExpression(expr->AST_left) + executeExpression(expr->AST_right);
        case OP_SUB:
            return executeExpression(expr->AST_left) - executeExpression(expr->AST_right);
        case OP_MUL:
            return executeExpression(expr->AST_left) * executeExpression(expr->AST_right);
        case OP_DIV:
            return executeExpression(expr->AST_left) / executeExpression(expr->AST_right);
        case OP_CALL:
            return callFunction(expr->AST_left, expr->AST_right->AST_list);
        default:
            fprintf(stderr, "unknown expression (type: %d)\n", expr->code);
            abort();
    }
}

int assign(Symbol *symbol, const int value) {
    // printf("assign: %s = %d\n", symbol->name, value);
    if (symbol->type != SYM_UNBOUND && symbol->type != SYM_VALUE) {
        fprintf(stderr, "Error: Attempt to assign to the unassignable variable %s.\n", symbol->name);
        abort();
    }
    symbol->type = SYM_VALUE;
    symbol->SYM_value = value;
    return value;
}

int executeAssign(AST *sym, AST *expr) {
    return assign(sym->AST_symbol, executeExpression(expr));
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

bool executeStatement(AST *ast) {
    switch (ast->code) {
        case ETC_LIST:
            executeStatements(ast->AST_list);
            break;
        case VAL_NUM:
            break;
        case VAL_SYMBOL:
            break;
        case CODE_ASSIGN:
            executeAssign(ast->AST_left, ast->AST_right);
            break;
        case CODE_PRINT:
            executePrint(ast);
            break;
        case CODE_RETURN:
            if (ast->AST_unary) {
                return_value = executeExpression(ast->AST_unary);
            }
            return false;
        case CODE_VAR:
            printf("declare variables...\n");
            break;
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
            break;
        case OP_CALL:
            executeExpression(ast);
            break;
        default:
            fprintf(stderr, "Unknown statement (type: %d)\n", ast->code);
            abort();
    }
    return true;
}

int resolveSymbol(const Symbol *symbol) {
    // find from environment
    StackIterator *iter = Stack_iterator(Env);
    while (StackIter_moveNext(iter)) {
        Environment *e = (Environment *)StackIter_current(iter);
        if (e->symbol->name == symbol->name) {
            StackIter_delete(iter);
            return e->value;
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

int bindSymbol(Symbol *symbol, const int value) {
    StackIterator *iter = Stack_iterator(Env);
    while (StackIter_moveNext(iter)) {
        Environment *e = (Environment *)StackIter_current(iter);
        if (e->symbol == symbol) {
            e->value = value;
            return value;
        }
    }
    return assign(symbol, value);
}

void AST_declareVariable(AST *symbol, AST *expr) {
    if (expr) {
        assign(symbol->AST_symbol, executeExpression(expr));
    } else {
    }
}

Environment *Environment_new(Symbol *symbol, int value) {
    Environment *env = (Environment *)malloc(sizeof(Environment));
    if (!env) {
        fprintf(stderr, "Cannot allocate memory for Environment.\n");
        abort();
    }
    env->symbol = symbol;
    env->value = value;
    return env;
}

void Environment_delete(Environment *env) {
    if (env) {
        env->symbol = NULL;
        env->value = 0;
        free(env);
    }
}

/* vim: set et ts=4 sts=4 sw=4: */
