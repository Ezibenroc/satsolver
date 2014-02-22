#include <set>
#include <iostream>
#include <cassert>

#include <exception>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "dpll.h"

using namespace satsolver;

bool verbose_dpll = false ;

void satsolver::process(Formula *formula, Affectation *affectation) {
    int literal;
    Formula *new_f ;
    if(formula->is_empty()) {
        return ;
    }

        // Unitary resolution
    literal = formula->find_monome() ;
    if(literal) {
        if(verbose_dpll) {
            std::cout << "Unitary resolution with literal " << literal << std::endl ;
        }
        formula->set_true(literal) ;
        affectation->set_true(literal) ;
        formula->clean() ;
        return process(formula,affectation) ;
    }
    
    // Isolated literal
    literal = formula->find_isolated_literal() ;
    if(literal) {
        if(verbose_dpll) {
            std::cout << "Isolated resolution with literal " << literal << std::endl ;
        }
        formula->set_true(literal) ;
        // Pas besoin d'appeler clean, il ne changerai pas la formule ici.
        affectation->set_true(literal) ;
        return process(formula,affectation) ;
    }
    
    // Make a bet
    literal = formula->choose_literal();
    try {
        if(verbose_dpll) {
            std::cout << "Make a bet with literal " << literal << std::endl ;
        }
        new_f = new Formula(formula) ;
        new_f->set_true(literal) ;
        affectation->set_true(literal) ; // pas la peine de copier l'affectation, on s'en sert seulement en écriture dans l'algo
        new_f->clean() ;
        process(new_f,affectation) ;
        delete new_f ; // suppression de new_f dans le cas où c'était satisfiable
    }
    catch (Conflict e) {
        delete new_f ; // suppression de new_f dans le cas où c'était insatisfiable
        if(verbose_dpll) {
            std::cout << "Made a wrong bet with literal " << literal << std::endl ;
        }
        formula->set_false(literal) ;
        formula->clean() ;
        affectation->set_false(literal) ;
        process(formula,affectation) ;
    }
}


Affectation* satsolver::solve(Formula *formula) {
    Affectation *aff = new Affectation(formula->get_nb_variables()) ;
    Formula *f = new Formula(formula) ;
    try {
        process(formula,aff) ;
    }
    catch (Conflict e) {
        delete f ;
        delete aff ;
        throw e ;
    }
    delete f ;
    return aff ;
}
