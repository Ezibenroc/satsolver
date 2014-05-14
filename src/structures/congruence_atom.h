#ifndef STRUCTURES_CONGRUENCE_ATOM_H
#define STRUCTURES_CONGRUENCE_ATOM_H

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace theorysolver {

class CongruenceAtom;

typedef std::vector<std::shared_ptr<CongruenceAtom>> CA_list;

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
        Term(const Term &t);

        bool operator==(const Term &that) const;
        bool operator!=(const Term &that) const { return !this->operator==(that); }

        std::string to_string() const;
};

class CongruenceAtom {
    public:
        enum Operator {
            EQUAL, UNEQUAL
        };
        //
        // Determines whether this variable represents an atom in the original extended formula.
        static bool is_atom_variable(std::string variable);
        // Determines whether this literal represents an atom in the original formula.
        static bool is_atom_literal(const std::map<int, std::string> &literal_to_name, unsigned int literal);
        // If literal represents an atom in the original formula, return its id.
        static unsigned int atom_id_from_literal(const std::map<int, std::string> &literal_to_name, unsigned int literal);
        // Return a variable name that can be used for representing the atom.
        static std::string variable_name_from_atom_id(unsigned long int atom_id);
        static int literal_from_atom_id(const std::map<std::string, int> &name_to_variable, unsigned int atom_id);
        // If literal represents an atom in the original formula, return this atom.
        static std::shared_ptr<CongruenceAtom> SPCA_from_literal(const CA_list &literal_to_CA, std::map<int, std::string> &literal_to_name, unsigned int literal);
        // If literal represents an atom in the original extended formula, return this atom.
        static std::shared_ptr<CongruenceAtom> SPCA_from_variable(const CA_list &literal_to_CA, std::string variable);
        // Add a CongruenceAtom and return its id.
        static long unsigned int add_CA(CA_list &literal_to_CA, Term &left, Operator op, Term &right);




        std::shared_ptr<Term> left, right;
        Operator op;

        CongruenceAtom(std::shared_ptr<Term> left, enum Operator op, std::shared_ptr<Term> right);
        std::string to_string() const;
};

}

#endif
