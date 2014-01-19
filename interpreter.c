#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "list.h"
#include "AST.h"

int yydebug;
int yyparse();
void executeProgram(const AST *ast);

bool iter_executeStmt(unsigned long i, const void *v);

int main() {
    yydebug = 1;
    SymbolTable = trie_new();
    int result = yyparse();
    executeProgram(Root);
    return result;
}

void executeProgram(const AST *ast) {
    assert(ast->code == ETC_LIST);
    List *list = ast->AST_list;
    List_each(list, iter_executeStmt);
}

int executeExpression(AST *expr) {
    switch (expr->code) {
        case VAL_NUM:
            return expr->AST_value;
        case VAL_SYMBOL:
            return expr->AST_symbol->SYM_value;
        case OP_ADD:
            return executeExpression(expr->AST_left) + executeExpression(expr->AST_right);
        case OP_SUB:
            return executeExpression(expr->AST_left) - executeExpression(expr->AST_right);
        case OP_MUL:
            return executeExpression(expr->AST_left) * executeExpression(expr->AST_right);
        case OP_DIV:
            return executeExpression(expr->AST_left) / executeExpression(expr->AST_right);
        default:
            fprintf(stderr, "expected expression.\n");
            abort();
    }
}

void executeAssign(AST *sym, AST *expr) {
    Symbol *symbol = sym->AST_symbol;
    symbol->SYM_value = executeExpression(expr);
}

void executePrint(AST *ast) {
    printf("%d\n", executeExpression(ast->AST_unary));
}

bool iter_executeStmt(unsigned long i, const void *v) {
    AST *ast = (AST *)v;
    switch (ast->code) {
        case ETC_LIST:
            List_each(ast->AST_list, iter_executeStmt);
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
        case OP_ADD:
            break;
        case OP_SUB:
            break;
        case OP_MUL:
            break;
        case OP_DIV:
            break;
        default:
            fprintf(stderr, "Unknown statement.\n");
            abort();
    }
    return true;
}

/* vim: set et ts=4 sts=4 sw=4: */
