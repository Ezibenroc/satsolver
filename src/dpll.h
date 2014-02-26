#ifndef DPLL_H
#define DPLL_H

#include "structures/affectation.h"
#include "structures/formula.h"

namespace satsolver {

    Affectation* solve(Formula *formula);


}

#endif
