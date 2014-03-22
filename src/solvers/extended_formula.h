#include <memory>
#include "structures/affectation.h"
#include "structures/extended_formula.h"

satsolver::Affectation solve_extended_formula(std::shared_ptr<satsolver::ExtendedFormula> ext_formula, std::shared_ptr<std::map<std::string, int>> *name_to_variable);

