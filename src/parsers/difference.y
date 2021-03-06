%{
#include <cstdio>
#include <memory>
#include <iostream>

#include "structures/extended_formula.h"
#include "structures/difference_atom.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define DA theorysolver::DifferenceAtom
#define SPDA std::shared_ptr<DA>

// Operator priority: NOT > AND > OR > IMPLIES

extern "C" {
int yylex(void);

extern int yylineno;

int yyerror(const char *);
}

void parser_result(SPEF formula, std::vector<SPDA> &literal_to_DA);

theorysolver::DA_list literal_to_DA;

%}

%error-verbose

%union { char *type_string; void *type_formula; int type_integer; int type_operator; void *type_atom; };

%token VAR
%token LEFT_PAREN RIGHT_PAREN
%token OR AND NOT IMPLIES

%token INT
%token MINUS LOWER GREATER LEQ GEQ EQUAL UNEQUAL

%start whole_formula

%type <type_formula> formula formula_not formula_and formula_or formula_implies atom
%type <type_string> VAR
%type <type_operator> difference_operator
%type <type_integer> INT
%type <type_atom> difference_atom

%%
whole_formula:
      formula { parser_result(SPEF((EF*) $1), literal_to_DA); }
    ;

difference_operator:
      LOWER { $$ = theorysolver::DifferenceAtom::LOWER; }
    | GREATER { $$ = theorysolver::DifferenceAtom::GREATER; }
    | LEQ { $$ = theorysolver::DifferenceAtom::LEQ; }
    | GEQ { $$ = theorysolver::DifferenceAtom::GEQ; }
    | EQUAL { $$ = theorysolver::DifferenceAtom::EQUAL; }
    | UNEQUAL { $$ = theorysolver::DifferenceAtom::UNEQUAL; }
    ;

difference_atom:
      VAR difference_operator INT { $$ = new DA(atoi($1+1), (theorysolver::DifferenceAtom::Operator) $2, $3); free($1); }
    | VAR MINUS VAR difference_operator INT { $$ = new DA(atoi($1+1), atoi($3+1), (theorysolver::DifferenceAtom::Operator) $4, $5); free($1); free($3); }
    ;

atom:
      VAR { $$ = new EF(EF::LITERAL, std::string($1)); free($1); }
    | difference_atom {
         literal_to_DA.push_back(SPDA((DA*) $1));
         $$ = new EF(EF::LITERAL, "#" + std::to_string(literal_to_DA.size()));
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

