#include <set>
#include <cassert>
#include <iostream>
#include <fstream>

#include "solver_main.h"
#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "parser.h"
#include "dpll.h"

int satsolver::solver_main(int argc, char *argv[], bool with_watched_literals) {
    satsolver::Parser *parser;
    std::istream *input;
    satsolver::Formula *formula;
    satsolver::Affectation *solution;
    std::set<int> *solution_set;
    std::set<int>::iterator solution_set_iterator;
    assert(!with_watched_literals); // Not implemented yet
    if (argc == 1) {
        input = &std::cin;
    }
    else if (argc == 2) {
        input = new std::ifstream(argv[1]);
    }
    else {
        std::cout << "Syntax: " << argv[0] << " [<filename>]\n\n";
        std::cout << "If filename is not given, stdin is used instead." << std::endl;
        return 1;
    }
    parser = new satsolver::Parser(*input);
    formula = parser->get_formula();
    try {
        solution = satsolver::solve(formula);
    }
    catch (satsolver::Conflict e) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return 1;
    }

    std::cout << "s SATISFIABLE" << std::endl;
    solution_set = solution->to_set();
		for (solution_set_iterator=solution_set->begin(); solution_set_iterator!=solution_set->end(); solution_set_iterator++) {
        std::cout << "v " << *solution_set_iterator << std::endl;
		}

    if (argc == 2)
        delete input;
    delete parser;
    delete solution ;
    delete formula ;
    delete solution_set ;
    return 0;
}
