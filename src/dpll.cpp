#include <set>
#include <iostream>
#include <cassert>

#include <exception>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "dpll.h"
#include "config.h"

using namespace satsolver;

Affectation* satsolver::solve(Formula *formula) {
    
    return formula->get_aff() ;
}
