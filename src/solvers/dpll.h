#ifndef DPLL_H
#define DPLL_H

#include "solvers/abstract_assistant.h"
#include "structures/affectation.h"
#include "structures/formula.h"

namespace satsolver {

    Affectation* solve(std::shared_ptr<Formula> formula);
    Affectation* solve(std::shared_ptr<Formula> formula, theorysolver::AbstractAssistant *assistant);


}

#endif
