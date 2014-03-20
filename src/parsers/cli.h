#ifndef PARSERS_CLI_H
#define PARSERS_CLI_H

#include <string>

#include "config.h"

class CommandLineParser {
    private:
        int nb_parsed_args;
        std::string extra_syntax;
    public:
        CommandLineParser(int argc, char *argv[], std::string extra_syntax);
        void print_syntax_error(char *executable);
        int get_nb_parsed_args();
};

#endif
