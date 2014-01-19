%token PRINT
%token SYMBOL
%token NUMBER
%token VAR

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

%type <val> program definitions
%type <val> statements statement expression
%type <val> assignment print_statement
%type <val> function_definition param_list symbol_list block
%type <val> variable_declaration variable_names
%type <val> SYMBOL NUMBER

%start program

%%
program:             { Root = NULL; }
       | definitions { Root = $1; }
definitions: function_definition
           | definitions function_definition

function_definition: SYMBOL '(' param_list ')' block { $$ = AST_makeFunction($1, $3, $5); }
param_list: /* no params */ { $$ = NULL; }
          | symbol_list     { $$ = $1; }
symbol_list: SYMBOL                 { $$ = AST_makeList($1); }
           | symbol_list ',' SYMBOL  { $$ = AST_addList($1, $3); }
block: '{' statements '}' { $$ = $2; }

statements: statement ';' { $$ = AST_makeList($1); }
          | statements statement ';' { $$ = AST_addList($1, $2); }
statement: assignment
         | print_statement
         | variable_declaration

assignment: SYMBOL '=' expression { $$ = AST_makeBinary(CODE_ASSIGN, $1, $3); }
print_statement: PRINT expression { $$ = AST_makeUnary(CODE_PRINT, $2); }
variable_declaration: VAR variable_names      { $$ = $2; }
variable_names: SYMBOL ';'                    { $$ = AST_makeList($1); }
              | variable_names ',' SYMBOL ';' { $$ = AST_addList($1, $3); }

expression: expression '+' expression { $$ = AST_makeBinary(OP_ADD, $1, $3); }
          | expression '-' expression { $$ = AST_makeBinary(OP_SUB, $1, $3); }
          | expression '*' expression { $$ = AST_makeBinary(OP_MUL, $1, $3); }
          | expression '/' expression { $$ = AST_makeBinary(OP_DIV, $1, $3); }
          | SYMBOL
          | NUMBER
          | '(' expression ')' { $$ = $2; }
%%

#include "lexer.c"
