#include "structures/difference_atom.h"

using namespace theorysolver;


DifferenceAtom::DifferenceAtom(unsigned int i, enum Operator op, unsigned int n) : i(i), j(0), op(op), n(n) {
}
DifferenceAtom::DifferenceAtom(unsigned int i, unsigned int j, enum Operator op, unsigned int n) : i(i), j(j), op(op), n(n) {
}

std::string DifferenceAtom::to_string() const {
    std::string s(std::to_string(this->i) + " "), op;
    if (this->j)
        s += std::to_string(this->j) + " ";
    switch (this->op) {
        case LOWER: op = "<"; break;
        case GREATER: op = ">"; break;
        case LEQ: op = "<="; break;
        case GEQ: op = ">="; break;
        case EQUAL: op = "="; break;
        case UNEQUAL: op = "!="; break;
    }
    s += op + " " + std::to_string(n);
    return s;
}
