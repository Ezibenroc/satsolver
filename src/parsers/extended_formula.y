%{
#include <cstdio>
#include <memory>
#include <iostream>

#include "structures/extended_formula.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>

// #define YYSTYPE SPEF

extern "C" {
int yylex(void);
int yyerror(const char *);
}

void parser_result(SPEF formula);

%}

%union { char *type_string; void *type_formula; };

%token VAR
%token LEFT_PAREN RIGHT_PAREN
%token OR AND NOT IMPLIES

%start whole_formula

%type <type_formula> formula
%type <type_string> VAR

%%
whole_formula:
      formula { parser_result(*(SPEF*) $1); }
    ;

formula:
      LEFT_PAREN formula RIGHT_PAREN { $$ = $2; }
    | VAR { $$ = new SPEF(new EF(EF::LITERAL, std::string($1))); }
    | NOT formula { $$ = new SPEF(new EF(EF::NOT, *(SPEF*) $2)); }
    | formula OR formula { $$ = new SPEF(new EF(EF::OR, *(SPEF*) $1, *(SPEF*) $3)); }
    | formula AND formula { $$ = new SPEF(new EF(EF::AND, *(SPEF*) $1, *(SPEF*) $3)); }
    | formula IMPLIES formula { $$ = new SPEF(new EF(EF::IMPLIES, *(SPEF*) $1, *(SPEF*) $3)); }
    ;

%%


extern "C" {
int yyerror (const char *msg) {
    return fprintf (stderr, "YACC: %s\n", msg);
}
}
