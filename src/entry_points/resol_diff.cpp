#include <set>
#include <string>
#include <memory>
#include <cassert>
#include <iostream>

#include "config.h"
#include "parsers/cli.h"
#include "solvers/dpll.h"
#include "solvers/tseitin.h"
#include "structures/difference_atom.h"
#include "structures/extended_formula.h"
#include "solvers/difference_assistant.h"
#include "extended_formula.y.hpp"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define DA theorysolver::DifferenceAtom
#define SPDA std::shared_ptr<DA>

extern FILE *yyin;
extern int yyparse();

bool VERBOSE = false;
bool WITH_WL = false;
bool DISPLAY_SAT;
bool DISPLAY_ATOMS;
bool DISPLAY_FORMULA;
satsolver::Heuristic HEURISTIC = satsolver::DUMB ;

void parser_result(SPEF ext_formula, std::vector<SPDA> &literal_to_DA) {
    /*********************
     * Reduce
     ********************/
    theorysolver::DifferenceAssistant *assistant;
    satsolver::Affectation *sat_solution;
    std::shared_ptr<std::map<std::string, int>> name_to_variable;
    std::shared_ptr<std::unordered_set<std::string>> literals = ext_formula->get_literals();
    std::shared_ptr<satsolver::Formula> formula;

    if (VERBOSE || DISPLAY_FORMULA)
        std::cout << "Interpreted formula as: " << ext_formula->to_string() << std::endl;
    if (VERBOSE || DISPLAY_ATOMS) {
        std::cout << "Atoms:" << std::endl;
        for (unsigned int i=0; i<literal_to_DA.size(); i++)
            std::cout << "\t#" << i << ": " << literal_to_DA[i]->to_string() << std::endl;
    }
    if (!tseitin_reduction(DISPLAY_SAT, ext_formula, name_to_variable, formula)) {
        // The formula is always false
        if (DISPLAY_SAT)
            std::cout << "c The formula is so obviously wrong it is not even needed to convert it to conjonctive form." << std::endl;
        std::cout << "s UNSATISFIABLE" << std::endl;
        return;
    }
    assistant = new theorysolver::DifferenceAssistant(literal_to_DA, formula);
    /*********************
     * Solve
     ********************/
    try {
        sat_solution = satsolver::solve(formula);
    }
    catch (satsolver::Conflict) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return;
    }
    /*********************
     * Display solution
     ********************/
    if (VERBOSE)
        std::cout << "Solution to SAT problem: " << sat_solution->to_string() << std::endl;
    for (auto literal : *literals) {
        std::cout << literal << " = " << (sat_solution->is_true(name_to_variable->at(literal)) ? "true" : "false") << std::endl;
    }
    delete assistant;
}

int main (int argc, char *argv[]) {
    /*********************
     * Get input
     ********************/
    CommandLineParser cli_parser(argc, argv, std::unordered_set<std::string>({"-print-interpretation", "-print-sat", "-print-atoms"}), "[-print-interpretation] [-print-sat] [-print-atoms] [<filename>]");
    if (cli_parser.get_nb_parsed_args() == -1)
        return 1;
    int nb_remaining_args = argc - cli_parser.get_nb_parsed_args();
    if (nb_remaining_args == 0) {
        // No other option
    }
    else if (nb_remaining_args == 1) {
        // One option left; hopefully the file name
        yyin = fopen(argv[argc-nb_remaining_args], "r");
    }
    else {
        // Too many unknown options
        cli_parser.print_syntax_error(argv[0]);
        return 1;
    }
    DISPLAY_SAT = cli_parser.get_arg("-print-sat");
    DISPLAY_ATOMS = cli_parser.get_arg("-print-atoms");
    DISPLAY_FORMULA = cli_parser.get_arg("-print-interpretation");
    return yyparse();
}

