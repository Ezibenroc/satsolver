#include <set>
#include <iostream>
#include <cassert>

#include <exception>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "dpll.h"

using namespace satsolver;

Formula* set_literal(const Formula *formula, int literal) {
    Formula *new_formula;
    new_formula = new Formula(formula);
    new_formula->set_true(literal);
    return new_formula;
}

Affectation* eliminate_monome(const Formula *formula, int literal) {
    Formula *new_formula = set_literal(formula, literal);
    return solve(new_formula);
}

Affectation* satsolver::solve(const Formula *formula) {
    int literal;
    literal = formula->find_monome();
    Affectation *affectation;
    if (formula->is_empty()) {
        return new Affectation(formula->get_nb_variables());
    }
    else if (literal) {
        affectation = eliminate_monome(formula, literal);
        affectation->set_true(literal);
        return affectation;
    }
    else if (formula->contains_empty_clause()) {
        throw Conflict();
    }
    else {
        literal = formula->choose_literal();
        try {
            // Make a bet
            affectation = solve(set_literal(formula, literal));
            affectation->set_true(literal);

        }
        catch (Conflict e) {
            // Bet was wrong
            affectation = solve(set_literal(formula, -literal));
            affectation->set_false(literal);
        }
        return affectation;
    }
}
