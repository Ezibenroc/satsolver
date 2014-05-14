#include <cassert>

#include "structures/congruence_atom.h"

using namespace theorysolver;

Term::Term(unsigned int symbol_id) : type(Term::SYMBOL), symbol_id(symbol_id), function_name(), arguments() {
}
Term::Term(std::string function_name, std::vector<std::shared_ptr<Term>> arguments) : type(Term::FUNCTION), symbol_id(0), function_name(function_name), arguments(arguments) {
}

bool Term::operator==(const Term &that) const {
    if (this->type != that.type)
        return false;
    else if (this->type == Term::SYMBOL)
        return this->symbol_id == that.symbol_id;
    else if (this->type == Term::FUNCTION) {
        if (this->arguments.size() != that.arguments.size())
            return false;
        if (this->function_name != that.function_name)
            return false;
        for (unsigned i=0; i<this->arguments.size(); i++)
            if (!(this->arguments[i] == that.arguments[i]))
                return false;
        return true;
    }
    else
        assert(false);
}



CongruenceAtom::CongruenceAtom(std::shared_ptr<Term> left, enum Operator op, std::shared_ptr<Term> right) : left(left), right(right), op(op) {
}

std::string CongruenceAtom::to_string() const {
    // TODO
    return std::string();
}
