%{
#include <cstdio>
#include <memory>
#include <iostream>

#include "structures/extended_formula.h"
#include "structures/equality_atom.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define EA theorysolver::EqualityAtom
#define SPEA std::shared_ptr<EA>

// Operator priority: NOT > AND > OR > IMPLIES

extern "C" {
int yylex(void);

extern int yylineno;

int yyerror(const char *);
}

void parser_result(SPEF formula, std::vector<SPEA> &literal_to_EA);

std::vector<SPEA> literal_to_EA;

%}

%error-verbose

%union { char *type_string; void *type_formula; int type_integer; void *type_atom; void *type_term; void *type_term_vector; };

%token VAR
%token LEFT_PAREN RIGHT_PAREN
%token OR AND NOT IMPLIES

%token F_SYMBOL COMMA ID INT
%token EQUAL UNEQUAL

%start whole_formula

%type <type_formula> formula formula_not formula_and formula_or formula_implies atom
%type <type_string> VAR
%type <type_integer> INT ID
%type <type_atom> equality_atom

%%
whole_formula:
      formula { parser_result(SPEF((EF*) $1), literal_to_EA); }
    ;


equality_atom:
      VAR EQUAL VAR { $$ = new EA(atoi($1+1), EA::EQUAL, atoi($3+1)); free($1); free($3); }
    | VAR UNEQUAL VAR { $$ = new EA(atoi($1+1), EA::UNEQUAL, atoi($3+1));; free($1); free($3); }
    ;



atom:
      VAR { $$ = new EF(EF::LITERAL, std::string($1)); free($1); }
    | equality_atom {
         literal_to_EA.push_back(SPEA((EA*) $1));
         $$ = new EF(EF::LITERAL, "#" + std::to_string(literal_to_EA.size()));
         }
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
    return fprintf (stderr, "YACC (line %d) : %s\n", yylineno, msg);
}
}

