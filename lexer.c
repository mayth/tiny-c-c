#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "AST.h"

int yylex() {
    int c;
    while (isspace(c = getc(stdin))) { }
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '(':
        case ')':
        case ';':
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
        if (strcmp(str, "print") == 0) {
            return PRINT;
        } else {
            yylval.val = AST_makeSymbol(str);
            return SYMBOL;
        }
        free(str);
    }
    return 0;
}

int yyerror(const char *s) {
  fprintf(stderr, "%s\n", s);
  return 0;
}
