#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "AST.h"

typedef struct {
    Symbol *symbol;
    int value;
} Environment;

static int return_value = 0;

int yydebug;
int yyparse();
int executeProgram(const AST *ast);
int executeFunction(const AST *body, List *param);
int callFunction(const AST *ast, List *args);
int callFunction_(const Symbol *sym, List *args);
int executeStatements(List *statements);
bool executeStatement(AST *ast);

int main() {
    yydebug = 1;
    SymbolTable = trie_new();
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

int executeFunction(const AST *body, List *params) {
    assert(body->code == ETC_LIST);
    executeStatements(body->AST_list);
    return return_value;
}

int callFunction_(const Symbol *sym, List *args) {
    return executeFunction(sym->SYM_body, sym->SYM_param);
}

int callFunction(const AST *ast, List *args) {
    Symbol *sym = ast->AST_symbol;
    if (sym->type == SYM_UNBOUND) {
        fprintf(stderr, "Used an undefined symbol: %s\n", ast->AST_symbol_name);
        abort();
    }
    return callFunction_(sym, args);
}

int executeExpression(AST *expr) {
    switch (expr->code) {
        case VAL_NUM:
            return expr->AST_value;
        case VAL_SYMBOL:
            if (expr->AST_symbol->type == SYM_UNBOUND) {
                fprintf(stderr, "Used undefined symbol: %s\n", expr->AST_symbol_name);
                abort();
            }
            return expr->AST_symbol->SYM_value;
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

void executeAssign(AST *sym, AST *expr) {
    Symbol *symbol = sym->AST_symbol;
    if (symbol->type != SYM_UNBOUND && symbol->type != SYM_VALUE) {
        fprintf(stderr, "Error: Attempt to assign to the unassignable variable: %s\n",
                sym->AST_symbol_name);
        abort();
    }
    symbol->type = SYM_VALUE;
    symbol->SYM_value = executeExpression(expr);
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

/* vim: set et ts=4 sts=4 sw=4: */
