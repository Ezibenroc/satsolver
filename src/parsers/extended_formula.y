%{
#include <cstdio>
#include <memory>
#include <iostream>

#include "structures/extended_formula.h"
#include "structures/difference_atom.h"
#include "structures/congruence_atom.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define DA theorysolver::DifferenceAtom
#define SPDA std::shared_ptr<DA>
#define CA theorysolver::CongruenceAtom
#define SPCA std::shared_ptr<CA>
#define TE theorysolver::Term 
#define SPTE std::shared_ptr<TE>

// Operator priority: NOT > AND > OR > IMPLIES

extern "C" {
int yylex(void);
int yyerror(const char *);
}

void parser_result(SPEF formula);

unsigned long int nb_DA;
std::vector<SPDA> literal_to_DA;
unsigned long int nb_CA;
std::vector<SPCA> literal_to_CA;
SPDA difference_atom;

%}

%union { char *type_string; void *type_formula; int type_integer; int type_operator; void *type_atom; void *type_term; void *type_term_vector; };

%token VAR
%token LEFT_PAREN RIGHT_PAREN
%token OR AND NOT IMPLIES

%token F_SYMBOL COMMA ID INT
%token MINUS LOWER GREATER LEQ GEQ EQUAL UNEQUAL

%start whole_formula

%type <type_formula> formula formula_not formula_and formula_or formula_implies atom
%type <type_string> VAR
%type <type_operator> difference_operator congruence_operator
%type <type_integer> INT ID
%type <type_atom> difference_atom congruence_atom
%type <type_term> term;
%type <type_term_vector> argument_list;

%%
whole_formula:
      formula { parser_result(SPEF((EF*) $1)); }
    ;

term:
      ID { $$ = new TE($1); } /* TODO: Make sure it is in the form xN */
    | VAR LEFT_PAREN argument_list RIGHT_PAREN { $$ = new TE(std::string($1), *((std::vector<SPTE>*) $3)); }

argument_list:
      term { $$ = new std::vector<SPTE>(); ((std::vector<SPTE>*) $$)->push_back(SPTE((TE*) $1)); }
    | argument_list COMMA term { $$ = $1; ((std::vector<SPTE>*) $1)->push_back(SPTE((TE*) $3)); }
    ;

congruence_operator:
      EQUAL { $$ = EQUAL; }
    | UNEQUAL { $$ = UNEQUAL; }
    ;

congruence_atom:
      term congruence_operator term { $$ = new CA(SPTE((TE*) $1), (theorysolver::CongruenceAtom::Operator) $2, SPTE((TE*) $3)); }
    ;

difference_operator:
      LOWER { $$ = LOWER; }
    | GREATER { $$ = GREATER; }
    | LEQ { $$ = LEQ; }
    | GEQ { $$ = GEQ; }
    | EQUAL { $$ = EQUAL; }
    | UNEQUAL { $$ = UNEQUAL; }
    ;

difference_atom:
      VAR difference_operator INT { $$ = new DA(atoi($1+1), (theorysolver::DifferenceAtom::Operator) $2, $3); }
    | VAR MINUS VAR difference_operator INT { $$ = new DA(atoi($1+1), atoi($3+1), (theorysolver::DifferenceAtom::Operator) $4, $5); }
    ;

atom:
      VAR { $$ = new EF(EF::LITERAL, std::string($1)); }
    | difference_atom {
         nb_DA = literal_to_DA.size();
         literal_to_DA.push_back(SPDA((DA*) $1));
         $$ = new EF(EF::LITERAL, "#" + std::to_string(nb_DA));
         }
    | congruence_atom {
         nb_CA = literal_to_CA.size();
         literal_to_CA.push_back(SPCA((CA*) $1));
         $$ = new EF(EF::LITERAL, "#" + std::to_string(nb_CA));
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
    return fprintf (stderr, "YACC: %s\n", msg);
}
}
