#include <set>
#include <string>
#include <memory>
#include <iostream>

#include "config.h"
#include "parsers/cli.h"
#include "solvers/dpll.h"
#include "structures/extended_formula.h"
#include "extended_formula.y.hpp"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>

extern FILE *yyin;
extern int yyparse();

bool VERBOSE = false;
bool WITH_WL = false;
bool DISPLAY_SAT;
bool DISPLAY_FORMULA;
satsolver::Heuristic HEURISTIC = satsolver::DUMB ;

void parser_result(SPEF ext_formula) {
    /*********************
     * Solve
     ********************/
    satsolver::Affectation *sat_solution;
    std::shared_ptr<std::map<std::string, int>> name_to_variable;
    std::shared_ptr<std::unordered_set<std::string>> literals = ext_formula->get_literals();

    if (VERBOSE || DISPLAY_FORMULA)
        std::cout << "Interpreted formula as: " << ext_formula->to_string() << std::endl;
    ext_formula = ext_formula->simplify();
    if (VERBOSE)
        std::cout << "Reduction of formula to: " << ext_formula->to_string() << std::endl;
    std::shared_ptr<satsolver::Formula> formula = NULL;
    try {
        formula = ext_formula->reduce_to_formula(&name_to_variable);
    }
    catch (satsolver::Conflict) {
    }

    /*********************
     * Display solution
     ********************/
    if (!formula) { // The formula is always false
        if (DISPLAY_SAT)
            std::cout << "c The formula is so obviously wrong it is not even needed to convert it to conjonctive form." << std::endl;
        std::cout << "s UNSATISFIABLE" << std::endl;
        return;
    }
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
    try {
        sat_solution = satsolver::solve(&*formula);
    }
    catch (satsolver::Conflict) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return;
    }
    if (VERBOSE)
        std::cout << "Solution to SAT problem: " << sat_solution->to_string() << std::endl;
    for (auto literal : *literals) {
        std::cout << literal << " = " << (sat_solution->is_true(name_to_variable->at(literal)) ? "true" : "false") << std::endl;
    }
}

int main (int argc, char *argv[]) {
    /*********************
     * Get input
     ********************/
    CommandLineParser cli_parser(argc, argv, std::unordered_set<std::string>({"-print-interpretation", "-print-sat"}), "[-print-sat] [<filename>]");
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
    DISPLAY_FORMULA = cli_parser.get_arg("-print-interpretation");
    return yyparse();
}
