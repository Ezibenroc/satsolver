#include <sstream>
#include "structures/color_affectation.h"

graphsolver::ColorAffectation::ColorAffectation(int nb_nodes, int *colors) : nb_nodes(nb_nodes), colors(colors) {
}
graphsolver::ColorAffectation::ColorAffectation(const graphsolver::ColorAffectation &that) : nb_nodes(that.nb_nodes), colors(that.colors) {
}
graphsolver::ColorAffectation& graphsolver::ColorAffectation::operator=(const graphsolver::ColorAffectation &that) {
    nb_nodes = that.nb_nodes;
    colors = that.colors;
    return *this;
}
graphsolver::ColorAffectation::~ColorAffectation() {
    free(this->colors);
}
std::string graphsolver::ColorAffectation::to_string() const {
    std::ostringstream oss;
    oss << "{" ;
    for(int i = 0 ; i < this->nb_nodes ; i++) {
        if (i)
            oss << ", ";
        oss << i << "=" << this->colors[i];
    }
    oss << "}" ;
    return oss.str() ;
}

// Get the graph coloration from the SAT solution.
std::shared_ptr<graphsolver::ColorAffectation> graphsolver::ColorAffectation::from_sat_solution(satsolver::Affectation *affectation, std::shared_ptr<std::map<std::string, int>> name_to_variable, int nb_nodes, int nb_bits) {
    int *colors = static_cast<int*>(malloc(sizeof(int)*nb_nodes));
    int i, j;
    int variable_id;
    for (i=0; i<nb_nodes; i++) {
        colors[i] = 0;
        for (j=nb_bits-1; j>=0; j--) {
            colors[i] <<= 1;
            try {
                variable_id = name_to_variable->at(get_variable_of_node_bit(i, j));
                if (affectation->is_true(variable_id))
                    colors[i]++;
            }
            catch (std::out_of_range) { // This variable does not occur in the formula
                // We could also increment color[i]++, it does not matter.
            }
        }
    }
    return std::make_shared<graphsolver::ColorAffectation>(nb_nodes, colors);
}

