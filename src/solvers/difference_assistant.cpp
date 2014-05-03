#include <cassert>
#include <iostream>
#include "solvers/difference_assistant.h"

using namespace theorysolver;

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define DA DifferenceAtom
#define SPDA std::shared_ptr<DA>

// Return a formula that uses only atoms in the canonical “xi-xj<=n” form.
// Note that it may use literal #0; for instance, xi >= n is converted to
// x0 - xi <= -n, so we will have to make sure x0 is always equal to 0.
SPEF canonize_atom(SPDA atom, std::vector<SPDA> &literal_to_DA, std::string literal) {
    unsigned long int id1, id2;
    assert(atom);
    switch (atom->op) {
        case DA::LOWER: // xi - xj < n   -->   xi - xj <= n-1
            literal_to_DA.push_back(SPDA(new DA(atom->i, atom->j, DA::LEQ, atom->n-1)));
            id1 = literal_to_DA.size();
            atom->canonical = SPEF(new EF(EF::LITERAL, "#" + std::to_string(id1)));
            break;
        case DA::LEQ: // xi - xj <= n   -->   xi - xj <= n
            atom->canonical = SPEF(new EF(EF::LITERAL, literal));
            break;
        case DA::GREATER: // xi - xj > n   -->   ~(xi - xj <= n)
            literal_to_DA.push_back(SPDA(new DA(atom->i, atom->j, DA::LEQ, atom->n)));
            id1 = literal_to_DA.size();
            atom->canonical = SPEF(new EF(EF::NOT,
                        SPEF(new EF(EF::LITERAL, "#" + std::to_string(id1)))
                   ));
            break;
        case DA::GEQ: // xi - xj >= n   -->   xj - xi <= -n
            literal_to_DA.push_back(SPDA(new DA(atom->j, atom->i, DA::LEQ, -atom->n)));
            id1 = literal_to_DA.size();
            atom->canonical = SPEF(new EF(EF::LITERAL, "#" + std::to_string(id1)));
            break;
        case DA::EQUAL: // xi - xj = n   -->   xi - xj <= n  ∧  xj - xi <= -n
            literal_to_DA.push_back(SPDA(new DA(atom->i, atom->j, DA::LEQ, atom->n)));
            id1 = literal_to_DA.size();
            literal_to_DA.push_back(SPDA(new DA(atom->j, atom->i, DA::LEQ, -atom->n)));
            id2 = literal_to_DA.size();
            atom->canonical = SPEF(new EF(EF::AND,
                SPEF(new EF(EF::LITERAL, "#" + std::to_string(id1))),
                SPEF(new EF(EF::LITERAL, "#" + std::to_string(id2)))));
            break;
        case DA::UNEQUAL: // xi - xj != n   -->   xi - xj <= n-1  ∨  xj - xi <= -n-1
            literal_to_DA.push_back(SPDA(new DA(atom->i, atom->j, DA::LEQ, atom->n-1)));
            id1 = literal_to_DA.size();
            literal_to_DA.push_back(SPDA(new DA(atom->j, atom->i, DA::LEQ, -atom->n-1)));
            id2 = literal_to_DA.size();
            atom->canonical = SPEF(new EF(EF::OR,
                SPEF(new EF(EF::LITERAL, "#" + std::to_string(id1))),
                SPEF(new EF(EF::LITERAL, "#" + std::to_string(id2)))));
            break;
    }
    return atom->canonical;
}

SPEF DifferenceAssistant::canonize_formula(SPEF formula, std::vector<SPDA> &literal_to_DA) {
    SPDA atom;
    switch (formula->get_type()) {
        case EF::XOR:
        case EF::OR:
        case EF::AND:
        case EF::IMPLIES:
            return std::make_shared<EF>(formula->get_type(), DifferenceAssistant::canonize_formula(formula->get_f1(), literal_to_DA), DifferenceAssistant::canonize_formula(formula->get_f2(), literal_to_DA));
        case EF::NOT:
            return std::make_shared<EF>(formula->get_type(), DifferenceAssistant::canonize_formula(formula->get_f1(), literal_to_DA));
        case EF::LITERAL:
            atom = literal_to_DA[atoi(formula->get_literal().c_str()+1)-1];
            if (formula->get_literal()[0] != '#')
                return std::make_shared<EF>(formula->get_type(), formula->get_literal());
            atom = literal_to_DA[atoi(formula->get_literal().c_str()+1)-1];
            return canonize_atom(atom, literal_to_DA, formula->get_literal());
        case EF::TRUE:
        case EF::FALSE:
            return std::make_shared<EF>(formula->get_type());
    }
    assert(false);
}


DifferenceAssistant::DifferenceAssistant(std::vector<SPDA> &literal_to_DA, std::shared_ptr<satsolver::Formula> formula) :
    formula(formula), literal_to_DA(literal_to_DA) {
}

bool DifferenceAssistant::on_flip(unsigned int variable) {
    (void) variable;
    return true;
}

bool DifferenceAssistant::is_state_consistent() {
    return true;
}
