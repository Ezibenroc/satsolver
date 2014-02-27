#include <set>
#include <iostream>
#include <cassert>

#include <exception>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "dpll.h"
#include "config.h"

using namespace satsolver;

Affectation* satsolver::solve(Formula *formula) {
    int literal, tmp ;
    bool contains_false_clause;
    while(formula->get_aff()->get_nb_unknown() != 0) { // tant qu'il reste des inconnus, on continue
    	literal = formula->isolated_literal() ;
    	if(literal) {
            if (WITH_WL) 
                contains_false_clause = !formula->deduce_true(literal);
            else {
                formula->deduce_true(literal);
                contains_false_clause = formula->contains_false_clause();
            }
    		while(contains_false_clause) {
                tmp = formula->back() ;
                if(tmp == 0)
                    throw Conflict() ;
                if (WITH_WL) 
                    contains_false_clause = !formula->deduce_false(tmp);
                else {
                    formula->deduce_false(tmp);
                    contains_false_clause = formula->contains_false_clause();
                }
    		}
    	}
    	else {
    		literal = formula->choose_literal() ;
            if (WITH_WL) 
                contains_false_clause = !formula->bet_true(literal);
            else {
                formula->bet_true(literal);
                contains_false_clause = formula->contains_false_clause();
            }
    		while(contains_false_clause){
                tmp = formula->back() ;
                if(tmp == 0)
                    throw Conflict() ;
                if (WITH_WL) 
                    contains_false_clause = !formula->deduce_false(tmp);
                else {
                    formula->deduce_false(tmp);
                    contains_false_clause = formula->contains_false_clause();
                }
			}
		}  	
    }
    return formula->get_aff() ;
}
