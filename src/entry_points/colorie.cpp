#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iomanip>
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

std::string to_hex(int i) {
    std::ostringstream oss;
    oss << std::setw(6) << std::setfill('0') << std::hex << i;
    return oss.str();
}

int main(int argc, char *argv[]) {
    bool using_stdin = true;
    std::istream *input = &std::cin;
    std::shared_ptr<graphsolver::ColorAffectation> solution;
    graphsolver::GraphParser *parser;
    int nb_colors;
    graphsolver::Graph *graph;
    int i;
    std::set<int>* nodes;
    int *colors, color;
    srand(static_cast<unsigned int>(time(NULL)));

    /*********************
     * Get input
     ********************/
    CommandLineParser cli_parser(argc, argv, std::unordered_set<std::string>({"-print-formula", "-print-sat"}), "[-print-formula] [-print-sat] <color> [<filename>]");
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
    DISPLAY_FORMULA = cli_parser.get_arg("-print-formula");
    DISPLAY_SAT = cli_parser.get_arg("-print-sat");

    /*********************
     * Solve
     ********************/
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

    /*********************
     * Display solution
     ********************/
    std::cout << "graph G {\n";
    colors = static_cast<int*>(malloc(sizeof(int)*nb_colors));
    memset(colors, 0, sizeof(int)*nb_colors);
    for (i=0; i<graph->get_nodes_count(); i++) {
        color = colors[solution->colors[i]];
        while (color <= 1) {
            color = rand() % (256*256*256);
        }
        colors[solution->colors[i]] = color;
        nodes = graph->get_lower_adjacent_nodes(i);
        std::cout << "\t" << i << " [color = \"#" << to_hex(color) << "\"];\n";
        for (auto j : *nodes)
            std::cout << "\t\t" << i << " -- " << j << ";\n";
    }
    std::cout << "}" << std::endl;
    free(colors);

    if (!using_stdin)
        delete input;
    delete parser;
    delete graph;
}
