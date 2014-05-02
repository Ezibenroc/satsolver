#include <cassert>
#include <iostream>
#include "solvers/difference_assistant.h"

using namespace theorysolver;

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define DA DifferenceAtom
#define SPDA std::shared_ptr<DA>

SPEF canonize_atom(SPDA atom, std::vector<SPDA> &literal_to_DA, std::string literal) {
    unsigned long int id = atom->opposite_id;
    assert(atom);
    switch (atom->op) {
        case DA::LOWER:
        case DA::LEQ:
        case DA::EQUAL:
            return SPEF(new EF(EF::LITERAL, literal));
        case DA::GREATER:
            if (!id) {
                literal_to_DA.push_back(SPDA(new DA(atom->i, atom->j, DA::GEQ, atom->n)));
                id = literal_to_DA.size();
                atom->opposite_id = id;
            }
            return SPEF(new EF(EF::NOT,
                        SPEF(new EF(EF::LITERAL,
                                    "#" + std::to_string(id)
                        ))
                   ));
        case DA::GEQ:
            if (!id) {
                literal_to_DA.push_back(SPDA(new DA(atom->i, atom->j, DA::LOWER, atom->n)));
                id = literal_to_DA.size();
                atom->opposite_id = id;
            }
            return SPEF(new EF(EF::NOT,
                        SPEF(new EF(EF::LITERAL,
                                    "#" + std::to_string(id)
                        ))
                   ));
        case DA::UNEQUAL:
            if (!id) {
                literal_to_DA.push_back(SPDA(new DA(atom->i, atom->j, DA::EQUAL, atom->n)));
                id = literal_to_DA.size();
                atom->opposite_id = id;
            }
            return SPEF(new EF(EF::NOT,
                        SPEF(new EF(EF::LITERAL,
                                    "#" + std::to_string(id)
                        ))
                   ));
    }
    assert(false);
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
