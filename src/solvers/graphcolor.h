#include <map>
#include <string>
#include <memory>

#include "structures/graph.h"
#include "structures/affectation.h"
#include "structures/color_affectation.h"

extern bool DISPLAY_FORMULA;
extern bool DISPLAY_SAT;

namespace graphsolver {

// Main function of the solver.
std::shared_ptr<ColorAffectation> solve_colors(int nb_colors, Graph *graph);

}

