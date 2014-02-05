#include <cassert>
#include <cstdlib>
#include <cstring>

#include "structures.h"

using namespace satsolver;

Clause::Clause(int nb_variables, bool *literals) {
    m_nb_variables = nb_variables;
    m_literals = literals;
}

Clause::Clause(int nb_variables, std::vector<int> literals) {
    m_literals = (bool*) malloc(nb_variables*2 + 1);
    memset(m_literals, false, nb_variables*2 + 1);
    for(std::vector<int>::iterator it = literals.begin(); it != literals.end(); ++it) {
        m_literals[*it] = true;
    }
}

bool Clause::contains_literal(int literal) {
    assert(literal+m_nb_variables<2*m_nb_variables);
    return m_literals[literal+m_nb_variables];
}
