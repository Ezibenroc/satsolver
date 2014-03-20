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
#include "config.h"

bool VERBOSE = false;
bool WITH_WL = false;
int HEURISTIC = DUMB ;

void bad_command_options(char *executable) {
    std::cout << "Syntax: " << executable << " [-verbose] [-WL] [-rand | -moms | -dlis] [<filename>]\n\n";
    std::cout << "If filename is not given, stdin is used instead." << std::endl;
}

int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL)) ;
    satsolver::SatParser *parser;
    satsolver::Formula *formula;
    satsolver::Affectation *solution;
    std::set<int> *solution_set;
    std::set<int>::iterator solution_set_iterator;
    int i;
    bool using_stdin = true;
    std::istream *input = &std::cin;
    for (i=1; i<argc && argv[i][0]=='-'; i++) {
        if (!strcmp(argv[i], "-verbose"))
            VERBOSE = true;
        else if (!strcmp(argv[i], "-WL"))
            WITH_WL = true;
        else if (!strcmp(argv[i], "-rand") && HEURISTIC == DUMB)
            HEURISTIC = RANDOM ;
        else if (!strcmp(argv[i], "-moms") && HEURISTIC == DUMB)
            HEURISTIC = MOMS ;
        else if (!strcmp(argv[i], "-dlis") && HEURISTIC == DUMB)
            HEURISTIC = DLIS ;
        else {
            bad_command_options(argv[0]);
            return 1;
        }
    }
    if (i == argc) {
        // No other option
    }
    else if (i == argc-1) {
        // One option left; hopefully the file name
        input = new std::ifstream(argv[i]);
        using_stdin = false;
    }
    else {
        // Too many unknown options
        bad_command_options(argv[0]);
        return 1;
    }
    parser = new satsolver::SatParser(*input);
    parser->parse();
    formula = parser->get_formula();
    try {
        solution = satsolver::solve(formula);
    }
    catch (satsolver::Conflict e) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return 0;
    }

    std::cout << "s SATISFIABLE" << std::endl;
    solution_set = solution->to_set();
    for (solution_set_iterator=solution_set->begin(); solution_set_iterator!=solution_set->end(); solution_set_iterator++) {
        std::cout << "v " << *solution_set_iterator << std::endl;
    }

    if (!using_stdin)
        delete input;
    delete parser;
    delete solution_set ;
    delete formula;
    return 0;
}
