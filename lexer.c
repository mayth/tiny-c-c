#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AST.h"

int take() {
    int c;
    while (isspace(c = getc(stdin))) { }
    return c;
}

int yylex() {
    int c = take();
    if (c == EOF) {
        return 0;
    }
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case ';':
            return c;
    }
    if (c == '=') {
        int next = getc(stdin);
        if (next == '=') {
            return OP_EQ;
        }
        ungetc(next, stdin);
        return c;
    }
    if (c == '<') {
        int next = getc(stdin);
        if (next == '=') {
            return OP_LE;
        }
        ungetc(next, stdin);
        return c;
    }
    if (c == '>') {
        int next = getc(stdin);
        if (next == '=') {
            return OP_GE;
        }
        ungetc(next, stdin);
        return c;
    }
    if (c == '!') {
        int next = getc(stdin);
        if (next == '=') {
            return OP_NEQ;
        }
        ungetc(next, stdin);
        return c;
    }
    if (isdigit(c)) {
        int n;
        for (n = 0; isdigit(c); c = getc(stdin)) {
            n = n * 10 + (c - '0');
        }
        ungetc(c, stdin);
        yylval.val = AST_makeValue(n);
        return NUMBER;
    }
    if (isalpha(c)) {
        char *str = (char *)malloc(sizeof(char) * MAX_SYMBOL_LEN);
        char *s = str;
        for (; isalpha(c); c = getc(stdin), ++s) {
            *s = c;
        }
        *s = '\0';
        ungetc(c, stdin);
        int retval = 0;
        if (strcmp(str, "print") == 0) {
            retval = PRINT;
        } else if (strcmp(str, "var") == 0) {
            retval = VAR;
        } else if (strcmp(str, "return") == 0) {
            retval = RETURN;
        } else if (strcmp(str, "for") == 0) {
            retval = FOR;
        } else {
            yylval.val = AST_makeSymbol(strdup(str));
            retval = SYMBOL;
        }
        free(str);
        return retval;
    }
    fprintf(stderr, "unexpected token %c\n", c);
    abort();
}

int yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
  return 0;
}

/* vim: set et ts=4 sts=4 sw=4: */
