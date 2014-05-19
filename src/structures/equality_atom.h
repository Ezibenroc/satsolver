#ifndef STRUCTURES_CONGRUENCE_ATOM_H
#define STRUCTURES_CONGRUENCE_ATOM_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include "structures/extended_formula.h"

namespace theorysolver {

class EqualityAtom;

typedef std::vector<std::shared_ptr<EqualityAtom>> EA_list;

class EqualityAtom {
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
        static std::shared_ptr<EqualityAtom> SPEA_from_literal(const EA_list &literal_to_EA, std::map<int, std::string> &literal_to_name, unsigned int literal);
        // If literal represents an atom in the original extended formula, return this atom.
        static std::shared_ptr<EqualityAtom> SPEA_from_variable(const EA_list &literal_to_EA, std::string variable);
        // Add a EqualityAtom and return its id.
        static long unsigned int add_EA(EA_list &literal_to_EA, unsigned int left, Operator op, unsigned int right);



        unsigned int left, right;
        Operator op;

        std::shared_ptr<satsolver::ExtendedFormula> canonical;

        EqualityAtom(unsigned int left, enum Operator op, unsigned int right);
        std::string to_string() const;
};

}

#endif
