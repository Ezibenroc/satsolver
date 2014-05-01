#ifndef DPLL_H
#define DPLL_H

#include "solvers/abstract_assistant.h"
#include "structures/affectation.h"
#include "structures/formula.h"

namespace satsolver {

    Affectation* solve(Formula *formula);
    Affectation* solve(Formula *formula, theorysolver::AbstractAssistant *assistant);


}

#endif
