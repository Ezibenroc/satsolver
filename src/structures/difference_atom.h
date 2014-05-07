#ifndef STRUCTURES_DIFFERENCE_ATOM_H
#define STRUCTURES_DIFFERENCE_ATOM_H

#include <string>
#include "structures/extended_formula.h"

namespace theorysolver {

class DifferenceAtom;

typedef std::vector<std::shared_ptr<DifferenceAtom>> DA_list;


class DifferenceAtom {
    public:
        enum Operator {
            LOWER, GREATER, LEQ, GEQ, EQUAL, UNEQUAL
        };

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
        static std::shared_ptr<DifferenceAtom> SPDA_from_literal(const DA_list &literal_to_DA, std::map<int, std::string> &literal_to_name, unsigned int literal);
        // If literal represents an atom in the original extended formula, return this atom.
        static std::shared_ptr<DifferenceAtom> SPDA_from_variable(const DA_list &literal_to_DA, std::string variable);
        // Add a DifferenceAtom and return its id.
        static long unsigned int add_DA(DA_list &literal_to_DA, unsigned int i, unsigned int j, Operator op, int n);


        unsigned int i, j;
        Operator op;
        int n;

        std::shared_ptr<satsolver::ExtendedFormula> canonical;

        DifferenceAtom(unsigned int i, enum Operator op, int n);
        DifferenceAtom(unsigned int i, unsigned int j, enum Operator op, int n);

        std::string to_string() const;
};

}

#endif
