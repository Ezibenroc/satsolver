#include "structures/difference_atom.h"

using namespace theorysolver;


DifferenceAtom::DifferenceAtom(unsigned int i, enum Operator op, unsigned int n) : i(i), j(0), op(op), n(n) {
}
DifferenceAtom::DifferenceAtom(unsigned int i, unsigned int j, enum Operator op, unsigned int n) : i(i), j(j), op(op), n(n) {
}
