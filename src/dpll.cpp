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
    while(formula->get_aff()->get_nb_unknown() != 0) { // tant qu'il reste des inconnus, on continue
    	literal = formula->isolated_literal() ;
    	if(literal) {
    		if(!formula->deduce_true(literal)) {
    			do {
    				tmp = formula->back() ;
    				if(tmp == 0)
    					throw Conflict() ;
    			}while(formula->deduce_false(tmp)) ;
    		}
    	}
    	else {
    		literal = formula->choose_literal() ;
    		if(!formula->bet_true(literal)){
    			do {
    				tmp = formula->back() ;
    				if(tmp == 0)
    					throw Conflict() ;
    			}while(formula->deduce_false(tmp)) ;
				}
			}
		}  	
    return formula->get_aff() ;
}
