#include <cassert>

#include "parsers/sat.h"

using namespace satsolver;

SatParser::SatParser(std::istream &stream) : AbstractParser::AbstractParser(stream), clauses(), variables_count(0), clauses_count(0), literals(), formula(NULL) {
}

satsolver::SatParser::SatParser(const satsolver::SatParser &that) : AbstractParser::AbstractParser(that), clauses(), variables_count(0), clauses_count(0), literals(), formula(that.formula) {
}

satsolver::SatParser& satsolver::SatParser::operator=(const satsolver::SatParser &that) {
    this->clauses = that.clauses;
    this->variables_count = that.variables_count;
    this->clauses_count = that.clauses_count;
    this->literals = that.literals;
    this->formula = formula;
    return *this;
}

SatParser::~SatParser() {
}

void SatParser::parse() {
    enum SatParser::State state = WAITING_FOR_HEADER;
    while (state != SatParser::END_OF_FILE)
        state = this->transition(state);
    this->formula = new Formula(this->clauses, this->variables_count);
}

enum SatParser::State SatParser::transition(enum SatParser::State state) {
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
                    return WAITING_FOR_FORMAT;
                case ' ':
                    return WAITING_FOR_HEADER;
                default:
                    throw SatParser::syntaxerror("Expected header.");
            };
            break;

        case WAITING_FOR_FORMAT: // In the header line, “p” has been read
            this->stream >> str;
            if (str != "cnf")
                throw SatParser::syntaxerror("Expected 'cnf'.");
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
                this->literals.insert(std::stoi(str));
            }
            catch (std::exception &e) {
                throw SatParser::syntaxerror(std::string() + "Expected integer, got '" + str + "'.");
            }
            return PARSING_CLAUSE;

        case PARSING_CLAUSE: // In a clause. At least one literal has been read.
            this->stream >> literal;
            if (literal) {
                this->literals.insert(literal);
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


    assert(false);
}

std::vector<std::shared_ptr<Clause>> SatParser::get_clauses() {
    return this->clauses;
}
Formula* SatParser::get_formula() {
    return this->formula;
}
int SatParser::get_variables_count() const {
    return this->variables_count;
}
int SatParser::get_clauses_count() const {
    return this->clauses_count;
}
