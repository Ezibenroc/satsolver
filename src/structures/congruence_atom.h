#ifndef STRUCTURES_CONGRUENCE_ATOM_H
#define STRUCTURES_CONGRUENCE_ATOM_H

#include <string>
#include <vector>
#include <memory>

namespace theorysolver {

class Term {
    public:
        enum Type {
            FUNCTION, SYMBOL
        };

        enum Type type;
        unsigned int symbol_id;
        std::string function_name;
        std::vector<std::shared_ptr<Term>> arguments;

        Term(unsigned int symbol_id);
        Term(std::string function_name, std::vector<std::shared_ptr<Term>> arguments);

        bool operator==(const Term &that) const;
        bool operator!=(const Term &that) const { return !this->operator==(that); }

        std::string to_string() const;
};

class CongruenceAtom {
    public:
        enum Operator {
            EQUAL, UNEQUAL
        };

        std::shared_ptr<Term> left, right;
        Operator op;

        CongruenceAtom(std::shared_ptr<Term> left, enum Operator op, std::shared_ptr<Term> right);
        std::string to_string() const;
};

}

#endif
