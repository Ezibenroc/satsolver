#include <vector>

namespace satsolver {

class Clause {
    private:
        int m_nb_variables;
        bool *m_literals;

    public:
        Clause(int nb_variables, bool *literals);
        Clause(int nb_variables, std::vector<int> literals);

        bool contains_literal(int literal);
};

}
