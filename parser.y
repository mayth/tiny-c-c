%token PRINT
%token VAR
%token RETURN
%token SYMBOL
%token NUMBER

%left '+' '-'
%left '*' '/'
%right '='

%{
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
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

%type <val> definitions definition
%type <val> statements statement expression
%type <val> assignment print_statement
%type <val> function_definition param_list symbol_list block
%type <val> variable_declaration variable_initialize variable_names
%type <val> argument_list arguments
%type <val> SYMBOL NUMBER

%start program

%%
program:
       | definitions 
definitions: definition { $$ = AST_makeList($1); }
           | definitions definition { $$ = AST_addList($1, $2); }

definition: function_definition
          | variable_declaration
          | variable_initialize

variable_declaration: VAR variable_names ';'  { }
variable_names: SYMBOL                        { AST_declareVariable($1, NULL); }
              | variable_names ',' SYMBOL     { AST_declareVariable($3, NULL); }
variable_initialize: VAR SYMBOL '=' expression ';' { AST_declareVariable($2, $4); }

function_definition: SYMBOL '(' param_list ')' block { $$ = AST_makeFunction($1, $3, $5); }
param_list: /* no params */ { $$ = AST_makeList(NULL); }
          | symbol_list     { $$ = $1; }
symbol_list: SYMBOL                 { $$ = AST_makeList($1); }
           | symbol_list ',' SYMBOL  { $$ = AST_addList($1, $3); }
block: '{' statements '}' { $$ = $2; }

statements: statement ';' { $$ = AST_makeList($1); }
          | statements statement ';' { $$ = AST_addList($1, $2); }
statement: expression
         | assignment
         | print_statement
         | variable_declaration
         | RETURN expression { $$ = AST_makeUnary(CODE_RETURN, $2); }
         | RETURN { $$ = AST_makeUnary(CODE_RETURN, NULL); }

assignment: SYMBOL '=' expression { $$ = AST_makeBinary(CODE_ASSIGN, $1, $3); }
print_statement: PRINT expression { $$ = AST_makeUnary(CODE_PRINT, $2); }

expression: NUMBER
          | SYMBOL /* variable ref */
          | expression '+' expression { $$ = AST_makeBinary(OP_ADD, $1, $3); }
          | expression '-' expression { $$ = AST_makeBinary(OP_SUB, $1, $3); }
          | expression '*' expression { $$ = AST_makeBinary(OP_MUL, $1, $3); }
          | expression '/' expression { $$ = AST_makeBinary(OP_DIV, $1, $3); }
          | SYMBOL '(' argument_list ')' { $$ = AST_makeBinary(OP_CALL, $1, $3); }
          | '(' expression ')' { $$ = $2; }

argument_list: /* no args */  { $$ = AST_makeList(NULL); }
             | arguments { $$ = $1; }
arguments: expression { $$ = AST_makeList($1); }
         | arguments ',' expression { $$ = AST_addList($1, $3); }
%%

#include "lexer.c"
