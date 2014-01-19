%token PRINT
%token SYMBOL
%token NUMBER
%left '+' '-'
%left '*' '/'
%right '='

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "trie.h"
#include "lexer.h"
#include "AST.h"
%}

%union {
  AST* val;
}

%type <val> statements statement expression
%type <val> assignment print_statement
%type <val> SYMBOL NUMBER

%start program

%%
program:
       | statements
statements: statement ';' { $$ = Root = AST_makeList($1); }
          | statements statement ';' { $$ = AST_addList($1, $2); }
statement: assignment
         | print_statement
assignment: SYMBOL '=' expression { $$ = AST_makeBinary(CODE_ASSIGN, $1, $3); }
print_statement: PRINT expression { $$ = AST_makeUnary(CODE_PRINT, $2); }
expression: expression '+' expression { $$ = AST_makeBinary(OP_ADD, $1, $3); }
          | expression '-' expression { $$ = AST_makeBinary(OP_SUB, $1, $3); }
          | expression '*' expression { $$ = AST_makeBinary(OP_MUL, $1, $3); }
          | expression '/' expression { $$ = AST_makeBinary(OP_DIV, $1, $3); }
          | SYMBOL
          | NUMBER
          | '(' expression ')' { $$ = $2; }
%%

#include "lexer.c"
