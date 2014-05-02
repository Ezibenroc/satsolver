#ifndef STRUCTURED_EXTENDED_FORMULE_H
#define STRUCTURED_EXTENDED_FORMULE_H

#include <unordered_set>
#include <string>
#include <memory>
#include <map>

#include "structures/formula.h"

namespace satsolver {

class ExtendedFormula {
    public:
        enum Type {
            XOR, OR, AND, IMPLIES, NOT, LITERAL,
            TRUE, FALSE
        };

    private:
        static int last_id; // Used to make unique literals

        enum Type type;

        std::shared_ptr<ExtendedFormula> f1;
        std::shared_ptr<ExtendedFormula> f2;
        std::shared_ptr<const ExtendedFormula> translation;
        std::string literal;
        int id;

        // Returns a unique literal name representing the clause.
        std::string make_literal() const;

    public:

        // Copy constructor
        ExtendedFormula(const ExtendedFormula *that);
        // TRUE/FALSE constructor
        ExtendedFormula(Type type);
        // LITERAL constructor
        ExtendedFormula(Type type, std::string literal);
        // NOT constructor
        ExtendedFormula(Type type, std::shared_ptr<ExtendedFormula> f);
        // XOR/OR/AND/IMPLIES constructor
        ExtendedFormula(Type type, std::shared_ptr<ExtendedFormula> f1, std::shared_ptr<ExtendedFormula> f2);

        // Returns a conjunction of disjonctions using Tseitin transformation
        std::vector<std::vector<std::string>*>* reduce_all() const;
        // Reduces this formula by appending the corresponding clauses to the
        // list of clauses and appends its childs to the list of formulas to
        // reduce.
        void reduce(std::vector<const ExtendedFormula*> *formulas, std::vector<std::vector<std::string>*> *clauses) const;
        // Calls reduce and returns a formula from it.
        std::shared_ptr<Formula> reduce_to_formula(std::shared_ptr<std::map<std::string, int>> *name_to_variable_ptr) const;

        // Returns a readable string representing the extended formula.
        std::string to_string() const;

        bool operator==(const ExtendedFormula &that) const;
        bool operator!=(const ExtendedFormula &that) const;

        // Replace trivial subtress (eg. A∧⊥ = A)
        std::shared_ptr<ExtendedFormula> simplify() const;
        // Returns a set of all literals in the formula.
        std::shared_ptr<std::unordered_set<std::string>> get_literals() const;

        Type get_type() const { return this->type; }
        std::shared_ptr<ExtendedFormula> get_f1() const { return this->f1; }
        std::shared_ptr<ExtendedFormula> get_f2() const { return this->f2; }
        std::string get_literal() const { return this->literal; }
};

}

#endif
