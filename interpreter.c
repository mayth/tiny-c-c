#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
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
int executeProgram(const AST *ast);
int executeFunction(const AST *body, List *args, List *param);
int callFunction(const AST *ast, List *args);
int callFunction_(const Symbol *sym, List *args);
int executeStatements(List *statements);
bool executeStatement(AST *ast);
int executeExpression(AST *expr);
int resolveSymbol(const Symbol *symbol);
int bindSymbol(Symbol *symbol, const int value);

int main() {
    yydebug = 0;
    SymbolTable = trie_new();
    Env = Stack_new();
    printf("*** start parsing ***\n");
    int result = yyparse();
    if (result != 0) {
        fprintf(stderr, "!!! Errors on parsing");
        return EXIT_FAILURE;
    }
    printf("*** parsed ***\n");
    if (Root) {
        printf("*** start execution ***\n");
        result = executeProgram(Root);
        printf("*** finished execution ***\n");
    } else {
        printf("!!! Root AST not found\n");
    }
    return result;
}

int executeProgram(const AST *ast) {
    Symbol *main = AST_lookupSymbolOrDie("main");
    if (main->type != SYM_FUNC) {
        fprintf(stderr, "'main' is not a function.\n");
        abort();
    }
    return callFunction_(main, NULL);
}

// args : List<Expression>
// params : List<Symbol>
int bindArgs(List *args, List *params) {
    unsigned long argc = List_size(args);
    if (argc != List_size(params)) {
        fprintf(stderr, "Argument Error (%lu args given, but expects %lu)\n", argc, List_size(params));
        abort();
    }
    ListIterator *args_iter = List_iterator(args);
    ListIterator *params_iter = List_iterator(params);
    while (ListIter_move_next(args_iter) && ListIter_move_next(params_iter)) {
        AST *expr = (AST *)ListIter_current(args_iter);
        Symbol *symbol = (Symbol *)ListIter_current(params_iter);
        Environment *e = Environment_new(symbol, executeExpression(expr));
        Stack_push(Env, e);
    }
    ListIter_delete(args_iter);
    ListIter_delete(params_iter);
    return argc;
}

void unbindArgs(const int argc) {
    for (int i = 0; i < argc; ++i) {
        Stack_pop(Env);
    }
}

int executeFunction(const AST *body, List *args, List *params) {
    assert(body->code == ETC_LIST);
    bool isExistParam = !List_is_empty(params);
    int argc = 0;
    if (isExistParam) bindArgs(args, params);
    executeStatements(body->AST_list);
    if (isExistParam) unbindArgs(argc);
    return return_value;
}

int callFunction_(const Symbol *sym, List *args) {
    return executeFunction(sym->SYM_body, args, sym->SYM_param);
}

int callFunction(const AST *ast, List *args) {
    Symbol *sym = ast->AST_symbol;
    if (sym->type == SYM_UNBOUND) {
        fprintf(stderr, "[func] Used an undefined symbol: %s\n", ast->AST_symbol_name);
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

int executeStatements(List *statements) {
    ListIterator *iter = List_iterator(statements);
    while (ListIter_move_next(iter)) {
        if (!executeStatement((AST *)ListIter_current(iter)))
            break;
    }
    ListIter_delete(iter);
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
        fprintf(stderr, "[expr] Used undefined symbol: %s\n", symbol->name);
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
