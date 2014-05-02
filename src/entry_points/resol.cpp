#include <set>
#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "parsers/sat.h"
#include "solvers/dpll.h"
#include "parsers/cli.h"
#include "config.h"

bool VERBOSE = false;
bool WITH_WL = false;
satsolver::Heuristic HEURISTIC = satsolver::DUMB ;

int main(int argc, char *argv[]) {
    srand(static_cast<unsigned int>(time(NULL)));
    satsolver::SatParser *parser;
    std::shared_ptr<satsolver::Formula> formula;
    satsolver::Affectation *solution;
    std::set<int> *solution_set;
    std::set<int>::iterator solution_set_iterator;
    bool using_stdin = true;
    std::istream *input = &std::cin;

    /*********************
     * Get input
     ********************/
    CommandLineParser cli_parser(argc, argv, std::unordered_set<std::string>(), "[<filename>]");
    if (cli_parser.get_nb_parsed_args() == -1)
        return 1;
    int nb_remaining_args = argc - cli_parser.get_nb_parsed_args();
    if (nb_remaining_args == 0) {
        // No other option
    }
    else if (nb_remaining_args == 1) {
        // One option left; hopefully the file name
        input = new std::ifstream(argv[argc-nb_remaining_args]);
        using_stdin = false;
    }
    else {
        // Too many unknown options
        cli_parser.print_syntax_error(argv[0]);
        return 1;
    }

    /*********************
     * Solve
     ********************/
    parser = new satsolver::SatParser(*input);
    try {
        parser->parse();
        formula = parser->get_formula();
        solution = satsolver::solve(formula);
    }
    catch (satsolver::Conflict e) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return 1;
    }

    /*********************
     * Display solution
     ********************/
    std::cout << "s SATISFIABLE" << std::endl;
    solution_set = solution->to_set();
    for (solution_set_iterator=solution_set->begin(); solution_set_iterator!=solution_set->end(); solution_set_iterator++) {
        std::cout << "v " << *solution_set_iterator << std::endl;
    }

    if (!using_stdin)
        delete input;
    delete parser;
    delete solution_set ;
    return 0;
}
