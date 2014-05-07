#include <memory>

#include "config.h"
#include "structures/formula.h"
#include "structures/extended_formula.h"

bool tseitin_reduction(bool DISPLAY_SAT, std::shared_ptr<satsolver::ExtendedFormula> ext_formula, std::shared_ptr<std::map<std::string, int>> &name_to_variable, std::shared_ptr<satsolver::Formula> &formula);
bool tseitin_reduction(bool DISPLAY_SAT, std::shared_ptr<satsolver::ExtendedFormula> ext_formula, std::shared_ptr<std::map<std::string, int>> &name_to_variable, std::shared_ptr<satsolver::Formula> &formula, std::vector<unsigned int> *affected_literals);

