#include <set>
#include <string>
#include <memory>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "config.h"
#include "parsers/cli.h"
#include "solvers/dpll.h"
#include "solvers/tseitin.h"
#include "structures/equality_atom.h"
#include "structures/extended_formula.h"
#include "solvers/equality_assistant.h"
#include "equality.y.hpp"
#include "utils.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define EA theorysolver::EqualityAtom
#define SPEA std::shared_ptr<EA>

extern FILE *yyin;
extern int yyparse();

bool VERBOSE = false;
bool WITH_WL = false;
bool DISPLAY_SAT;
bool DISPLAY_ATOMS;
bool DISPLAY_FORMULA;
satsolver::Heuristic HEURISTIC = satsolver::DUMB ;

void parser_result(SPEF ext_formula, std::vector<SPEA> &literal_to_EA) {
    /*********************
     * Reduce
     ********************/
    theorysolver::EqualityAssistant *assistant;
    satsolver::Affectation *sat_solution;
    std::vector<unsigned int> affected_literals;
    std::shared_ptr<std::map<std::string, int>> name_to_variable;
    std::map<int, std::string> variable_to_name;
    std::shared_ptr<std::unordered_set<std::string>> literals = ext_formula->get_literals();
    std::shared_ptr<satsolver::Formula> formula;

    if (VERBOSE || DISPLAY_FORMULA)
        std::cout << "Interpreted formula as: " << ext_formula->to_string() << std::endl;
    if (VERBOSE || DISPLAY_ATOMS) {
        std::cout << "Atoms:" << std::endl;
        for (unsigned int i=0; i<literal_to_EA.size(); i++)
            std::cout << "\t#" << i+1 << ": " << literal_to_EA[i]->to_string() << std::endl;
    }
    ext_formula = theorysolver::EqualityAssistant::canonize_formula(ext_formula, literal_to_EA);
    if (VERBOSE || DISPLAY_FORMULA)
        std::cout << "Canonized formula as: " << ext_formula->to_string() << std::endl;
    if (VERBOSE || DISPLAY_ATOMS) {
        std::cout << "Atoms:" << std::endl;
        for (unsigned int i=0; i<literal_to_EA.size(); i++)
            std::cout << "\t#" << i+1 << ": " << literal_to_EA[i]->to_string() << std::endl;
    }
    if (!tseitin_reduction(DISPLAY_SAT, ext_formula, name_to_variable, formula, &affected_literals)) {
        // The formula is always false
        if (DISPLAY_SAT)
            std::cout << "c The formula is so obviously wrong it is not even needed to convert it to conjonctive form." << std::endl;
        std::cout << "s UNSATISFIABLE" << std::endl;
        return;
    }
    if (!name_to_variable) {
        switch (ext_formula->get_type()) {
            case EF::TRUE:
                std::cout << "s SATISFIABLE (tautology)" << std::endl;
                return;
            case EF::FALSE:
                std::cout << "s UNSATISFIABLE" << std::endl;
                return;
            default:
                std::cout << "s SATISFIABLE" << std::endl;
                return;
        }
    }
    assistant = new theorysolver::EqualityAssistant(literal_to_EA, name_to_variable, formula);
    for (auto it : affected_literals) {
        if (-1!=assistant->on_flip(it)) {
            std::cout << "s UNSATISFIABLE" << std::endl;
            return;
        }
    }

    /*********************
     * Solve
     ********************/
    try {
        sat_solution = satsolver::solve(formula, assistant);
    }
    catch (satsolver::Conflict) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return;
    }
    /*********************
     * Display solution
     ********************/
    assert(assistant->is_state_consistent());
    if (VERBOSE) {
        std::cout << "Solution to SAT problem: " << sat_solution->to_string() << std::endl;
        /* std::cout << assistant->get_graph().to_string() << std::endl; */
    }
    for (auto literal : *literals) {
        std::cout << literal;
        unsigned long offset = literal.size();
        std::string aff;
        try {
            aff = sat_solution->is_true(name_to_variable->at(literal)) ? "true" : "false";
            std::cout << ": ";
        }
        catch (std::out_of_range) {
            SPEF f = literal_to_EA[atoi(literal.c_str()+1)-1]->canonical;
            std::cout << " (inferred from " << f->to_string() << ")" << ": ";
            offset += unisize((" (inferred from "+f->to_string()+")").c_str());
            try {
                aff = f->is_true(formula->get_aff(), name_to_variable) ? "true" :" false";
            }
            catch (std::out_of_range) {
                std::cout << "can be true or false" << std::endl ;
            }
        }
        for (unsigned long i=offset; i<40; i++)
            std::cout << " ";
        std::cout << literal_to_EA[atoi(literal.c_str()+1)-1]->to_string() << " = " << aff << std::endl;

    }

    std::cout << "Affectation of variables: " << std::endl;
    for (unsigned i=1; i<assistant->get_max_node(); i++)
        std::cout << "\tx" << std::to_string(i) << " = \t" << assistant->get_component(i) << std::endl;
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

