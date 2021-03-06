#ifndef PARSERS_SAT_H
#define PARSERS_SAT_H

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include <exception>

#include "structures/clause.h"
#include "structures/formula.h"
#include "parsers/abstract.h"

namespace satsolver {

// Finite state machine
class SatParser : public AbstractParser {
    public:
        enum State {
            WAITING_FOR_HEADER,
            WAITING_FOR_FORMAT,
            WAITING_FOR_VARIABLES_COUNT,
            WAITING_FOR_CLAUSES_COUNT,

            WAITING_FOR_CLAUSE,
            PARSING_CLAUSE,

            IN_COMMENT,
            IN_COMMENT_BEFORE_HEADER,

            END_OF_FILE
        };

    private:

        std::vector<std::shared_ptr<Clause>> clauses;
        int variables_count, clauses_count;
        std::unordered_set<int> literals;
        std::shared_ptr<Formula> formula;

        enum State transition(enum State);


    public:
        SatParser(std::istream &stream);
        SatParser(const SatParser &that);
        SatParser& operator=(const SatParser&);
        virtual ~SatParser();
        virtual void parse();

        std::vector<std::shared_ptr<Clause>> get_clauses();
        std::shared_ptr<Formula> get_formula();
        int get_variables_count() const;
        int get_clauses_count() const;
};

}

#endif
