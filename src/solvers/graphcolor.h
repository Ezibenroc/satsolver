#include <map>
#include <string>
#include <memory>

#include "structures/graph.h"
#include "structures/affectation.h"

extern bool DISPLAY_FORMULA;
extern bool DISPLAY_SAT;

namespace graphsolver {

class ColorAffectation {
    public:
        int nb_nodes, *colors;

        ColorAffectation(int nb_nodes, int *colors);
        ~ColorAffectation();
        std::string to_string() const;

        // Deduces the color affectation from a SAT solution and the literal
        // names translation map.
        static std::shared_ptr<ColorAffectation> from_sat_solution(satsolver::Affectation *affectation, std::shared_ptr<std::map<std::string, int>> name_to_variable, int nodes_count, int nb_bits);
};

// Main function of the solver.
std::shared_ptr<ColorAffectation> solve_colors(int nb_colors, Graph *graph);

}

