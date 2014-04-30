#include "structures/congruence_atom.h"

using namespace theorysolver;

Term::Term(unsigned int symbol_id) : type(Term::SYMBOL), symbol_id(symbol_id), function_name(), arguments() {
}
Term::Term(std::string function_name, std::vector<std::shared_ptr<Term>> arguments) : type(Term::FUNCTION), symbol_id(0), function_name(function_name), arguments(arguments) {
}

CongruenceAtom::CongruenceAtom(std::shared_ptr<Term> left, enum Operator op, std::shared_ptr<Term> right) : left(left), right(right), op(op) {
}
