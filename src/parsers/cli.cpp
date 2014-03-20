#include <cstring>
#include <iostream>
#include "parsers/cli.h"

CommandLineParser::CommandLineParser(int argc, char *argv[], std::string extra_syntax) : extra_syntax(extra_syntax) {
    int i;
    for (i=1; i<argc && argv[i][0]=='-'; i++) {
        if (!strcmp(argv[i], "-verbose"))
            VERBOSE = true;
        else if (!strcmp(argv[i], "-WL"))
            WITH_WL = true;
        else if (!strcmp(argv[i], "-rand") && HEURISTIC == satsolver::DUMB)
            HEURISTIC = satsolver::RANDOM ;
        else if (!strcmp(argv[i], "-moms") && HEURISTIC == satsolver::DUMB)
            HEURISTIC = satsolver::MOMS ;
        else if (!strcmp(argv[i], "-dlis") && HEURISTIC == satsolver::DUMB)
            HEURISTIC = satsolver::DLIS ;
        else {
            this->print_syntax_error(argv[0]);
            this->nb_parsed_args = -1;
            return;
        }
    }
    this->nb_parsed_args = i;
}
void CommandLineParser::print_syntax_error(char *executable) {
    std::cout << "Syntax: " << executable << " [-verbose] [-WL] [-rand | -moms | -dlis] " << this->extra_syntax << "\n\n";
    std::cout << "If filename is not given, stdin is used instead." << std::endl;
}
int CommandLineParser::get_nb_parsed_args() {
    return this->nb_parsed_args;
}
