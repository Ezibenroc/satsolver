#include <map>
#include <string>
#include <memory>

#include "structures/graph.h"
#include "structures/affectation.h"
#include "structures/color_affectation.h"

namespace graphsolver {

// Main function of the solver.
std::shared_ptr<ColorAffectation> solve_colors(int nb_colors, Graph *graph);

}

