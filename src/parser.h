#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <exception>

#include "structures/clause.h"
#include "structures/formula.h"

namespace satsolver {

// Finite state machine
class Parser {
    public:
        enum State {
            WAITING_FOR_HEADER,
            WAITING_FOR_CNF,
            WAITING_FOR_VARIABLES_COUNT,
            WAITING_FOR_CLAUSES_COUNT,

            WAITING_FOR_CLAUSE,
            PARSING_CLAUSE,

            IN_COMMENT,
            IN_COMMENT_BEFORE_HEADER,

            END_OF_FILE,
        };

        class syntaxerror : public std::exception {
            private:
                const char *msg;
            public:
                syntaxerror(const char *_msg) : msg(_msg) {}
                syntaxerror(const std::string _msg) : msg(_msg.c_str()) {}
                syntaxerror() : msg("<unknown error>") {}
                virtual const char* what() const throw() {
                    std::cout << std::string(this->msg);
                    std::cout << std::endl;
                    return this->msg;
                }
        };


    private:

        std::vector<std::shared_ptr<Clause>> clauses;
        int variables_count, clauses_count;
        std::istream &stream;
        std::vector<int> literals;
        Formula *formula;

        void parse();
        enum State transition(enum State);


    public:
        Parser(std::istream &stream);
        ~Parser();

        std::vector<std::shared_ptr<Clause>> get_clauses();
        Formula* get_formula();
        int get_variables_count() const;
        int get_clauses_count() const;
};

}

#endif
