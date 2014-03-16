#include <cstring>
#include <fstream>
#include <iostream>

#include "structures/graph.h"
#include "parsers/graph.h"
#include "solvers/graphcolor.h"
#include "solvers/dpll.h"
#include "config.h"

bool VERBOSE = false;
bool WITH_WL = false;
int HEURISTIC = DUMB ;

void bad_command_options(char *executable) {
    std::cout << "Syntax: " << executable << " [-verbose] [<filename>]\n\n";
    std::cout << "If filename is not given, stdin is used instead." << std::endl;
}


int main(int argc, char *argv[]) {
    bool using_stdin = true;
    std::istream *input = &std::cin;
    graphsolver::ColorAffectation *solution;
    graphsolver::GraphParser *parser;
    int nb_colors;
    graphsolver::Graph *graph;
    int i;
    for (i=1; i<argc && argv[i][0]=='-'; i++) {
        if (!strcmp(argv[i], "-verbose")) {
            VERBOSE = true;
        }
        else {
            bad_command_options(argv[0]);
            return 1;
        }
    }
    if (i == argc-1) {
        // No other option
        nb_colors = atoi(argv[i]);
    }
    else if (i == argc-2) {
        // One option left; hopefully the file name
        nb_colors = atoi(argv[i]);
        input = new std::ifstream(argv[i+1]);
        using_stdin = false;
    }
    else {
        // Too many unknown options
        bad_command_options(argv[0]);
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
}
