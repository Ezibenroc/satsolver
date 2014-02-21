#include <set>
#include <iostream>
#include <cassert>

#include <exception>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "dpll.h"

using namespace satsolver;

void satsolver::process(Formula *formula, Affectation *affectation) {
    int literal;
    Formula *new_f ;
    if(formula->is_empty()) {
    	return ;
    }

		// Unitary resolution
    literal = formula->find_monome() ;
    if(literal) {
    	formula->set_true(literal) ;
    	affectation->set_true(literal) ;
    	process(formula,affectation) ;
    }
    
    // Isolated literal
    literal = formula->find_isolated_literal() ;
    if(literal) {
    	formula->set_true(literal) ;
    	affectation->set_true(literal) ;
    	process(formula,affectation) ;
    }
    
    // Make a bet
    literal = formula->choose_literal();
    try {
    	new_f = new Formula(*formula) ;
    	new_f->set_true(literal) ;
    	affectation->set_true(literal) ; // pas la peine de copier l'affectation, on s'en sert seulement en écriture dans l'algo
    	process(new_f,affectation) ;
    }
    catch (Conflict e) {
    	delete new_f ;
    	formula->set_false(literal) ;
    	affectation->set_false(literal) ;
    	process(formula,affectation) ;
    }
}


Affectation* satsolver::solve(Formula *formula) {
	Affectation *aff = new Affectation(formula->get_nb_variables()) ;
	process(formula,aff) ;
	return aff ;
}
