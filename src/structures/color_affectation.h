#include <memory>
#include <map>

#include "structures/affectation.h"

#define get_variable_of_node_bit(node_id, bit_id) (std::to_string(node_id) + " " + std::to_string(bit_id))

namespace graphsolver {

class ColorAffectation {
    public:
        int nb_nodes, *colors;

        ColorAffectation(int nb_nodes, int *colors);
        ColorAffectation(const ColorAffectation &that);
        ColorAffectation& operator=(const ColorAffectation &that);
        ~ColorAffectation();
        std::string to_string() const;

        // Deduces the color affectation from a SAT solution and the literal
        // names translation map.
        static std::shared_ptr<ColorAffectation> from_sat_solution(satsolver::Affectation *affectation, std::shared_ptr<std::map<std::string, int>> name_to_variable, int nodes_count, int nb_bits);
};

}
