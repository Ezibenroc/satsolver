#include <map>
#include <memory>

#include "structures/graph.h"
#include "structures/affectation.h"

namespace graphsolver {

class ColorAffectation {
    public:
        int nb_nodes, *colors;

        ColorAffectation(int nb_nodes, int *colors);

        static ColorAffectation* from_sat_solution(satsolver::Affectation *affectation, std::shared_ptr<std::map<std::string, int>> name_to_variable, int nodes_count, int nb_bits);
};

ColorAffectation* solve_colors(int nb_colors, Graph *graph);

}

