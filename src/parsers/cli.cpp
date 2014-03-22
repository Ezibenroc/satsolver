#include <cstring>
#include <iostream>
#include <unordered_set>
#include <string>
#include "parsers/cli.h"

CommandLineParser::CommandLineParser(int argc, char *argv[], std::unordered_set<std::string> extra_args, std::string extra_syntax) : extra_syntax(extra_syntax) {
    int i;
    for (auto arg : extra_args)
        this->extra_args.insert(std::pair<std::string, bool>(arg, false));
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
        else if (extra_args.find(argv[i]) != extra_args.end())
            this->extra_args.find(argv[i])->second = true;
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

bool CommandLineParser::get_arg(std::string arg) {
    return this->extra_args.at(arg);
}
