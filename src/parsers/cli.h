#ifndef PARSERS_CLI_H
#define PARSERS_CLI_H

#include <map>
#include <unordered_set>
#include <string>

#include "config.h"

class CommandLineParser {
    private:
        int nb_parsed_args;
        std::map<std::string, bool> extra_args;
        std::string extra_syntax;
    public:
        CommandLineParser(int argc, char *argv[], std::unordered_set<std::string> extra_args, std::string extra_syntax);
        void print_syntax_error(char *executable);
        int get_nb_parsed_args();
        bool get_arg(std::string arg);
};

#endif
