#include <memory>
#include <iostream>

#include "config.h"
#include "structures/extended_formula.h"
#include "extended_formula.y.hpp"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>

bool VERBOSE = false;
bool WITH_WL = false;
satsolver::Heuristic HEURISTIC = satsolver::DUMB ;

void parser_result(SPEF formula) {
    std::cout << formula->to_string() << std::endl;
}

int main (void) {
    return yyparse();
}
