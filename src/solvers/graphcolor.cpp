#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <stdexcept>

#include "structures/extended_formula.h"
#include "solvers/dpll.h"
#include "solvers/graphcolor.h"
#include "config.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>

bool DISPLAY_FORMULA = false;
bool DISPLAY_SAT = false;

// Get a disjonction of XORs saying “I don't want these two nodes to be the
// same color”.
SPEF get_color_disjonction_of_edge(int first_node_id, int second_node_id, int nb_bits) {
    int i;
    SPEF literal1, literal2, formula2;
    SPEF formula = SPEF(new EF(EF::FALSE));
    for (i=0; i<nb_bits; i++) {
        literal1 = SPEF(new EF(EF::LITERAL, get_variable_of_node_bit(first_node_id, i)));
        literal2 = SPEF(new EF(EF::LITERAL, get_variable_of_node_bit(second_node_id, i)));
        formula2 = SPEF(new EF(EF::XOR, literal1, literal2));
        formula = SPEF(new EF(EF::OR, formula, formula2));
    }
    return formula;
}

// Get a disjonction saying “I don't want the color identifier of the node
// to be greater or equal to X”.
SPEF get_color_limitation_formula_for_node(int node_id, int nb_colors, int nb_bits) {
    int i;
    SPEF formula, this_bit, not_this_bit;
    nb_colors--;
    if (nb_colors % 2)
        formula = SPEF(new EF(EF::TRUE));
    else
        formula = SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, get_variable_of_node_bit(node_id, 0)))));
    for (i=1; i<nb_bits; i++) {
        nb_colors >>= 1;
        this_bit = SPEF(new EF(EF::LITERAL, get_variable_of_node_bit(node_id, i)));
        not_this_bit = SPEF(new EF(EF::NOT, this_bit));
        if (nb_colors % 2) {
            // (¬“node_id i”)+((“node_id i”∧formula))
            formula = SPEF(new EF(EF::OR,
                                  not_this_bit,
                                  SPEF(new EF(EF::AND,
                                              this_bit,
                                              formula
                                  ))
                            ));
        }
        else {
            formula = SPEF(new EF(EF::AND, not_this_bit, formula));
        }
    }
    return formula;
}

// Conjonction of all formulas returned by all calls (one per node) to
// get_color_limitation_formula_for_node.
SPEF get_color_limitation_formula(int nb_nodes, int nb_colors, int nb_bits) {
    int i;
    SPEF formula = SPEF(new EF(EF::TRUE));
    SPEF new_formula;
    for (i=0; i<nb_nodes; i++) {
        formula = SPEF(new EF(EF::AND, formula, get_color_limitation_formula_for_node(i, nb_colors, nb_bits)));
    }
    return formula;
}

// Returns an extended formula from a graph coloration problem.
int reduce_graph_coloration_to_extended_formula(graphsolver::Graph *graph, int nb_colors, SPEF *formula) {
    int nb_bits = static_cast<int>(ceil(log2(nb_colors)));
    int nodes_count = graph->get_nodes_count();
    int i;
    std::set<int> *adjacent_nodes;
    *formula = get_color_limitation_formula(graph->get_nodes_count(), nb_colors, nb_bits);
    for (i=0; i<nodes_count; i++) {
        adjacent_nodes = graph->get_lower_adjacent_nodes(i);
        for (auto adjacent_node : *adjacent_nodes) {
            *formula = SPEF(new EF(EF::AND, get_color_disjonction_of_edge(i, adjacent_node, nb_bits), *formula));
        }
    }
    return nb_bits;
}

// Main function of the solver.
std::shared_ptr<graphsolver::ColorAffectation> graphsolver::solve_colors(int nb_colors, Graph *graph) {
    int nb_bits;
    SPEF ext_formula;
    std::shared_ptr<satsolver::Formula> formula;
    satsolver::Affectation *sat_solution;
    std::shared_ptr<graphsolver::ColorAffectation> color_affectation;
    std::shared_ptr<std::map<std::string, int>> name_to_variable;

    nb_bits = reduce_graph_coloration_to_extended_formula(graph, nb_colors, &ext_formula);
    if (VERBOSE || DISPLAY_FORMULA)
        std::cout << "Reduction of graph coloring problem to: " << ext_formula->to_string() << std::endl;
    ext_formula = ext_formula->simplify();
    if (VERBOSE || DISPLAY_FORMULA)
        std::cout << "Reduction of formula to: " << ext_formula->to_string() << std::endl;
    formula = ext_formula->reduce_to_formula(&name_to_variable);
    if (!formula) // The formula is always false
        throw satsolver::Conflict();
    if (VERBOSE)
        std::cout << "Reduction of formula to SAT: " << formula->to_string() << std::endl;
    if (DISPLAY_SAT) {
        std::cout << "c Start of formula\n" ;
        std::cout << formula->to_string2();
        for (int i=1; i<=formula->get_nb_variables(); i++) {
            if (formula->get_aff()->is_true(i))
                std::cout << i << " 0\n";
            else if (formula->get_aff()->is_false(i))
                std::cout << -i << " 0\n";
        }
        std::cout << "c End of formula\n" << std::endl ;
    }
    sat_solution = satsolver::solve(formula); // May raise a satsolver::Conflict
    if (VERBOSE)
        std::cout << "Solution to SAT problem: " << sat_solution->to_string() << std::endl;
    color_affectation = ColorAffectation::from_sat_solution(sat_solution, name_to_variable, graph->get_nodes_count(), nb_bits);
    if (VERBOSE)
        std::cout << "Color affectation: " << color_affectation->to_string() << std::endl;
    return color_affectation;
}
