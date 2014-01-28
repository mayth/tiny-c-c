%token PRINT
%token VAR
%token RETURN
%token FOR
%token SYMBOL
%token NUMBER
%token OP_EQ
%token OP_NEQ
%token OP_LE
%token OP_GE

%right '='
%left '+' '-'
%left '*' '/'
%nonassoc OP_EQ OP_NEQ OP_LE OP_GE '<' '>'

%{
#define YYDEBUG 1
#define YYERROR_VERBOSE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "lexer.h"
#include "AST.h"
%}

%union {
  AST* val;
}

%type <val> global_definitions global_definition
%type <val> statements statement expression
%type <val> stmt_or_block block
%type <val> none_or_statements
%type <val> print_statement for_statement
%type <val> function_definition param_list symbol_list
%type <val> variable_names
%type <val> argument_list arguments
%type <val> SYMBOL NUMBER

%start program

%%
program:
       | global_definitions 
global_definitions: global_definition { $$ = AST_makeList($1); }
                  | global_definitions global_definition { $$ = AST_addList($1, $2); }

global_definition: function_definition
                 | VAR variable_names ';'        { }
                 | VAR SYMBOL '=' expression ';' { AST_initializeVariable($2, $4); }
                 | VAR SYMBOL '[' expression ']' ';' { AST_declareArray($2, $4); }

variable_names: SYMBOL
              | variable_names ',' SYMBOL

function_definition: SYMBOL '(' param_list ')' block { $$ = AST_makeFunction($1, $3, $5); }
param_list: /* no params */ { $$ = AST_makeList(NULL); }
          | symbol_list     { $$ = $1; }
symbol_list: SYMBOL                 { $$ = AST_makeList($1); }
           | symbol_list ',' SYMBOL  { $$ = AST_addList($1, $3); }
block: '{' none_or_local_declarations none_or_statements '}' { $$ = $3; }

stmt_or_block: statement { $$ = AST_makeList($1); }
             | block

none_or_local_declarations:
                          | local_declarations

local_declarations: local_declaration
                  | local_declarations local_declaration

local_declaration: VAR variable_names ';'
                 | VAR SYMBOL '=' expression ';'
                 | VAR SYMBOL '[' expression ']' ';'

none_or_statements:             { $$ = AST_makeList(NULL); }
                  | statements  { $$ = $1; }

statements: statement { $$ = AST_makeList($1); }
          | statements statement { $$ = AST_addList($1, $2); }
statement: expression ';'
         | print_statement ';'
         | RETURN expression ';' { $$ = AST_makeUnary(CODE_RETURN, $2); }
         | RETURN ';' { $$ = AST_makeUnary(CODE_RETURN, NULL); }
         | for_statement

print_statement: PRINT expression { $$ = AST_makeUnary(CODE_PRINT, $2); }

for_statement: FOR '(' expression ';' expression ';' expression ')' stmt_or_block
             { $$ = AST_makeFor($3, $5, $7, $9); }

expression: NUMBER
          | SYMBOL /* variable ref */
          | SYMBOL '[' expression ']' { $$ = AST_makeBinary(OP_REF_ARRAY, $1, $3); }
          | SYMBOL '=' expression     { $$ = AST_makeBinary(OP_ASSIGN, $1, $3); }
          | SYMBOL '[' expression ']' '=' expression { $$ = AST_makeTrinary(OP_ASSIGN_ARRAY, $1, $3, $6); }
          | expression '+' expression { $$ = AST_makeBinary(OP_ADD, $1, $3); }
          | expression '-' expression { $$ = AST_makeBinary(OP_SUB, $1, $3); }
          | expression '*' expression { $$ = AST_makeBinary(OP_MUL, $1, $3); }
          | expression '/' expression { $$ = AST_makeBinary(OP_DIV, $1, $3); }
          | expression '<' expression { $$ = AST_makeBinary(OP_COMPARE_LT, $1, $3); }
          | expression '>' expression { $$ = AST_makeBinary(OP_COMPARE_GT, $1, $3); }
          | expression OP_LE expression { $$ = AST_makeBinary(OP_COMPARE_LE, $1, $3); }
          | expression OP_GE expression { $$ = AST_makeBinary(OP_COMPARE_GE, $1, $3); }
          | expression OP_EQ expression { $$ = AST_makeBinary(OP_COMPARE_EQ, $1, $3); }
          | expression OP_NEQ expression { $$ = AST_makeBinary(OP_COMPARE_NEQ, $1, $3); }
          | SYMBOL '(' argument_list ')' { $$ = AST_makeBinary(OP_CALL, $1, $3); }
          | '(' expression ')' { $$ = $2; }

argument_list: /* no args */  { $$ = AST_makeList(NULL); }
             | arguments { $$ = $1; }
arguments: expression { $$ = AST_makeList($1); }
         | arguments ',' expression { $$ = AST_addList($1, $3); }
%%

#include "lexer.c"
