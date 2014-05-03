#ifndef STRUCTURES_DIFFERENCE_ATOM_H
#define STRUCTURES_DIFFERENCE_ATOM_H

#include <string>
#include "structures/extended_formula.h"

namespace theorysolver {

class DifferenceAtom {
    public:
        enum Operator {
            LOWER, GREATER, LEQ, GEQ, EQUAL, UNEQUAL
        };

        unsigned int i, j;
        Operator op;
        int n;

        std::shared_ptr<satsolver::ExtendedFormula> canonical;

        DifferenceAtom(unsigned int i, enum Operator op, int n);
        DifferenceAtom(unsigned int i, unsigned int j, enum Operator op, int n);

        std::string to_string() const;
};

}

#endif
