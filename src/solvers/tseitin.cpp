#include <cassert>
#include <iostream>
#include "solvers/tseitin.h"

bool tseitin_reduction(bool DISPLAY_SAT, std::shared_ptr<satsolver::ExtendedFormula> ext_formula, std::shared_ptr<std::map<std::string, int>> &name_to_variable, std::shared_ptr<satsolver::Formula> &formula) {
    return tseitin_reduction(DISPLAY_SAT, ext_formula, name_to_variable, formula, NULL);
}
bool tseitin_reduction(bool DISPLAY_SAT, std::shared_ptr<satsolver::ExtendedFormula> ext_formula, std::shared_ptr<std::map<std::string, int>> &name_to_variable, std::shared_ptr<satsolver::Formula> &formula, std::vector<unsigned int> *affected_literals) {
    ext_formula = ext_formula->simplify();
    if (VERBOSE)
        std::cout << "Reduction of formula to: " << ext_formula->to_string() << std::endl;
    try {
        formula = ext_formula->reduce_to_formula(&name_to_variable, affected_literals);
    }
    catch (satsolver::Conflict) {
        return false;
    }
    if (!formula) {
        formula = std::shared_ptr<satsolver::Formula>(new satsolver::Formula({std::shared_ptr<satsolver::Clause>(new satsolver::Clause(1, {1}))}, 1));
        return true;
    }

    if (VERBOSE)
        std::cout << "Reduction of formula to SAT: " << formula->to_string() << std::endl;
    if (DISPLAY_SAT) {
        std::cout << "c Start of formula\n" ;
        std::cout << formula->to_string2();
        for (int i=1; i<=formula->get_nb_variables(); i++) {
            if (formula->get_aff()->is_true(i))
                std::cout << i << " 0\n";
            else if (formula->get_aff()->is_false(i))
                std::cout << -i << " 0\n";
        }
        std::cout << "c End of formula\n" << std::endl ;
    }
    return true;
}

