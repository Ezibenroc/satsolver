#include <set>
#include <iostream>
#include <cassert>

#include <exception>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "solvers/dpll.h"
#include "config.h"

using namespace satsolver;

Affectation* satsolver::solve(Formula *formula) {
    int literal, tmp ;
    bool contains_false_clause;
    while(formula->get_aff()->get_nb_unknown() != 0 && !formula->only_true_clauses(NULL)) {
        if(!WITH_WL && (literal = formula->monome())) {
            formula->deduce_true(literal, NULL);
            contains_false_clause = formula->contains_false_clause(NULL);
        }
        else if((literal = formula->isolated_literal())) {
            if (WITH_WL)
                contains_false_clause = !formula->deduce_true(literal, NULL);
            else {
                formula->deduce_true(literal, NULL);
                contains_false_clause = formula->contains_false_clause(NULL);
            }
        }
        else {
            literal = formula->choose_literal(HEURISTIC) ;
            if (WITH_WL)
                contains_false_clause = !formula->bet_true(literal, NULL);
            else {
                formula->bet_true(literal, NULL);
                contains_false_clause = formula->contains_false_clause(NULL);
            }
        }
        while(contains_false_clause){
            tmp = formula->back() ;
            if(tmp == 0)
                throw Conflict() ;
            if (WITH_WL)
                contains_false_clause = !formula->deduce_false(tmp, NULL);
            else {
                formula->deduce_false(tmp, NULL);
                contains_false_clause = formula->contains_false_clause(NULL);
            }
        }
    }
    return formula->get_aff() ;
}
