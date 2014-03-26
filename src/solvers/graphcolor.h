#include <map>
#include <string>
#include <memory>

#include "structures/graph.h"
#include "structures/affectation.h"

namespace graphsolver {

class ColorAffectation {
    public:
        int nb_nodes, *colors;

        ColorAffectation(int nb_nodes, int *colors);
        ~ColorAffectation();
        std::string to_string() const;

        static std::shared_ptr<ColorAffectation> from_sat_solution(satsolver::Affectation *affectation, std::shared_ptr<std::map<std::string, int>> name_to_variable, int nodes_count, int nb_bits);
};

std::shared_ptr<ColorAffectation> solve_colors(int nb_colors, Graph *graph);

}
