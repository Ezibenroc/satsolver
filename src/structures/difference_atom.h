#ifndef STRUCTURES_DIFFERENCE_ATOM_H
#define STRUCTURES_DIFFERENCE_ATOM_H

#include <string>

namespace theorysolver {

class DifferenceAtom {
    public:
        enum Operator {
            LOWER, GREATER, LEQ, GEQ, EQUAL, UNEQUAL
        };

        unsigned int i, j;
        Operator op;
        unsigned int n;

        unsigned long int opposite_id;

        DifferenceAtom(unsigned int i, enum Operator op, unsigned int n);
        DifferenceAtom(unsigned int i, unsigned int j, enum Operator op, unsigned int n);

        std::string to_string() const;
};

}

#endif
