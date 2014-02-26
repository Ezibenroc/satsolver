#include <cassert>

#include "parser.h"

using namespace satsolver;

Parser::Parser(std::istream &_stream) : stream(_stream) {
    this->parse();
}
Parser::~Parser() {
}

void Parser::parse() {
    enum Parser::State state = WAITING_FOR_HEADER;
    while (state != Parser::END_OF_FILE)
        state = this->transition(state);
    this->formula = new Formula(this->clauses, this->variables_count);
}

enum Parser::State Parser::transition(enum Parser::State state) {
    char prefix;
    std::string str;
    int literal;
    switch (state) {

        case WAITING_FOR_HEADER: // At the beginning of a line, header not seen yet
            this->stream >> prefix;
            switch (prefix) {
                case 'c':
                    return IN_COMMENT_BEFORE_HEADER;
                case 'p':
                    return WAITING_FOR_CNF;
                case ' ':
                    return WAITING_FOR_HEADER;
                default:
                    throw Parser::syntaxerror("Expected header.");
            };
            break;

        case WAITING_FOR_CNF: // In the header line, “p” has been read
            this->stream >> str;
            if (str != "cnf")
                throw Parser::syntaxerror("Expected 'cnf'.");
            return WAITING_FOR_VARIABLES_COUNT;

        case WAITING_FOR_VARIABLES_COUNT: // In the header line, “p cnf” has been read
            this->stream >> this->variables_count;
            return WAITING_FOR_CLAUSES_COUNT;

        case WAITING_FOR_CLAUSES_COUNT: // In the header line, “p cnf X” (where X is the variables count) has been read
            this->stream >> this->clauses_count;
            return WAITING_FOR_CLAUSE;

        case WAITING_FOR_CLAUSE: // At the beginning of a line, header has already been seen.
            if (this->stream.eof())
                return END_OF_FILE;
            this->stream >> str;
            if (str == "c")
                return IN_COMMENT;
            else if (str == "" || str == "\n")
                return WAITING_FOR_CLAUSE;
            try {
                this->literals.push_back(std::stoi(str));
            }
            catch (std::exception &e) {
                throw Parser::syntaxerror(std::string() + "Expected integer, got '" + str + "'.");
            }
            return PARSING_CLAUSE;

        case PARSING_CLAUSE: // In a clause. At least one literal has been read.
            this->stream >> literal;
            if (literal) {
                this->literals.push_back(literal);
                return PARSING_CLAUSE;
            }
            else {
                std::getline(this->stream, str); // Consume remaining characters
                this->clauses.push_back(std::shared_ptr<Clause>(new Clause(this->variables_count, this->literals)));
                this->literals.clear();
                return WAITING_FOR_CLAUSE;
            }

        case IN_COMMENT: // In a comment. Header has already been seen.
            std::getline(this->stream, str);
            return WAITING_FOR_CLAUSE;

        case IN_COMMENT_BEFORE_HEADER: // In a command. Header has not been seen yet.
            std::getline(this->stream, str);
            return WAITING_FOR_HEADER;

        case END_OF_FILE:
            // We have read the EOF character and set the state to END_OF_FILE, which is a terminal state and therefore this case should never be matched.
            assert(false);

    }


    return state;
}

std::vector<std::shared_ptr<Clause>> Parser::get_clauses() {
    return this->clauses;
}
Formula* Parser::get_formula() {
    return this->formula;
}
int Parser::get_variables_count() const {
    return this->variables_count;
}
int Parser::get_clauses_count() const {
    return this->clauses_count;
}
