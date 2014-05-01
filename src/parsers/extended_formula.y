%{
#include <cstdio>
#include <memory>
#include <iostream>

#include "structures/extended_formula.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>

// Operator priority: NOT > AND > OR > IMPLIES

extern "C" {
int yylex(void);

int yyerror(const char *);
}

void parser_result(SPEF formula);

%}

%error-verbose

%union { char *type_string; void *type_formula; };

%token VAR
%token LEFT_PAREN RIGHT_PAREN
%token OR AND NOT IMPLIES

%start whole_formula

%type <type_formula> formula formula_not formula_and formula_or formula_implies atom
%type <type_string> VAR

%%
whole_formula:
      formula { parser_result(SPEF((EF*) $1)); }
    ;


atom:
      VAR { $$ = new EF(EF::LITERAL, std::string($1)); }
    ;

formula_not:
      LEFT_PAREN formula RIGHT_PAREN { $$ = $2; }
    | atom { $$ = $1; }
    | NOT formula_not { $$ = new EF(EF::NOT, SPEF((EF*) $2)); }
formula_and:
      formula_not { $$ = $1; }
    | formula_not AND formula_and { $$ = new EF(EF::AND, SPEF((EF*) $1), SPEF((EF*) $3)); }
    ;
formula_or:
      formula_and { $$ = $1; }
    | formula_and OR formula_or { $$ = new EF(EF::OR, SPEF((EF*) $1), SPEF((EF*) $3)); }
    ;
formula_implies:
      formula_or { $$ = $1; }
    | formula_or IMPLIES formula_implies { $$ = new EF(EF::IMPLIES, SPEF((EF*) $1), SPEF((EF*) $3)); }
    ;
formula:
      formula_implies { $$ = $1; }
    ;

%%


extern "C" {
int yyerror (const char *msg) {
    return fprintf (stderr, "YACC : %s\n", msg);
}
}
