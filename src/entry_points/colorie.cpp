#include <cstring>
#include <fstream>
#include <iostream>

#include "structures/graph.h"
#include "parsers/graph.h"
#include "parsers/cli.h"
#include "solvers/graphcolor.h"
#include "solvers/dpll.h"
#include "config.h"

bool VERBOSE = false;
bool WITH_WL = false;
satsolver::Heuristic HEURISTIC = satsolver::DUMB ;


int main(int argc, char *argv[]) {
    bool using_stdin = true;
    std::istream *input = &std::cin;
    graphsolver::ColorAffectation *solution;
    graphsolver::GraphParser *parser;
    int nb_colors;
    graphsolver::Graph *graph;
    CommandLineParser cli_parser(argc, argv, "<color> [<filename>]");
    if (cli_parser.get_nb_parsed_args() == -1)
        return 1;
    int nb_remaining_args = argc - cli_parser.get_nb_parsed_args();
    if (nb_remaining_args == 1 && atoi(argv[argc-nb_remaining_args])) {
        // One option left; hopefully the number of colors
        nb_colors = atoi(argv[argc-nb_remaining_args]);
    }
    else if (nb_remaining_args == 2) {
        // One option left; hopefully the number of colors and the file name
        nb_colors = atoi(argv[argc-nb_remaining_args]);
        input = new std::ifstream(argv[argc-nb_remaining_args+1]);
        using_stdin = false;
    }
    else {
        // Too many unknown options
        cli_parser.print_syntax_error(argv[0]);
        return 1;
    }
    parser = new graphsolver::GraphParser(*input);
    parser->parse();
    graph = parser->get_graph();
    try {
        solution = graphsolver::solve_colors(nb_colors, graph);
    }
    catch (satsolver::Conflict e) {
        std::cout << "graph G { n [label=\"No solution.\"] }" << std::endl;
        return 1;
    }

    if (!using_stdin)
        delete input;
    delete solution;
    delete parser;
    delete graph;
}
