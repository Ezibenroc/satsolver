#include <iostream>

#include "solvers/extended_formula.h"
#include "structures/formula.h"
#include "solvers/dpll.h"
#include "config.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>

satsolver::Affectation solve_extended_formula(SPEF ext_formula, std::shared_ptr<std::map<std::string, int>> *name_to_variable) {
    std::shared_ptr<satsolver::Formula> formula;
    satsolver::Affectation *sat_solution;
    ext_formula = ext_formula->simplify();
    if (VERBOSE)
        std::cout << "Reduction of formula to: " << ext_formula->to_string() << std::endl;
    formula = ext_formula->reduce_to_formula(name_to_variable);
    if (!formula) // The formula is always false
        throw satsolver::Conflict();
    if (VERBOSE)
        std::cout << "Reduction of formula to SAT: " << formula->to_string() << std::endl;
    sat_solution = satsolver::solve(&*formula); // May raise a satsolver::Conflict
    if (VERBOSE)
        std::cout << "Solution to SAT problem: " << sat_solution->to_string() << std::endl;

    return *sat_solution;
}
