#ifndef STRUCTURES_DIFFERENCE_ATOM_H
#define STRUCTURES_DIFFERENCE_ATOM_H

namespace theorysolver {

class DifferenceAtom {
    public:
        enum Operator {
            LOWER, GREATER, LEQ, GEQ, EQUAL, UNEQUAL
        };

        unsigned int i, j;
        Operator op;
        unsigned int n;

        DifferenceAtom(unsigned int i, enum Operator op, unsigned int n);
        DifferenceAtom(unsigned int i, unsigned int j, enum Operator op, unsigned int n);
};

}

#endif
