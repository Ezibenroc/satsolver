#include <cassert>
#include <iostream>
#include <climits>
#include "solvers/congruence_assistant.h"
#include "config.h"

using namespace theorysolver;

#define VERBOSE_ASSISTANT false

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define CA CongruenceAtom
#define SPCA std::shared_ptr<CA>

// Return a formula that uses only atoms in the canonical “xi-xj<=n” form.
// Note that it may use literal #0; for instance, xi >= n is converted to
// x0 - xi <= -n, so we will have to make sure x0 is always equal to 0.
SPEF canonize_atom(SPCA atom, std::vector<SPCA> &literal_to_CA, std::string literal) {
    unsigned long int id1;
    assert(atom);
    if(atom->left==atom->right) {
        switch (atom->op) {
            case CA::EQUAL: // t1 = t1   
                atom->canonical = SPEF(new EF(EF::TRUE));
                break;
            case CA::UNEQUAL: // t1 != t1  
                atom->canonical = SPEF(new EF(EF::FALSE));
                break;      
        } 
    }
    else {
        switch (atom->op) {
            case CA::EQUAL: // t1 = t2   --> t1 = t2
                atom->canonical = SPEF(new EF(EF::LITERAL, literal));
                break;
            case CA::UNEQUAL: // t2 != t2   -->   ~(t1 = t2)
                id1 = CongruenceAtom::add_CA(literal_to_CA, *atom->left, CA::EQUAL, *atom->right);
                atom->canonical = SPEF(new EF(EF::NOT,
                            SPEF(new EF(EF::LITERAL, CongruenceAtom::variable_name_from_atom_id(id1)))
                       ));
                break;
        }
    }
    return atom->canonical;
}

SPEF CongruenceAssistant::canonize_formula(SPEF formula, std::vector<SPCA> &literal_to_CA) {
    SPCA atom;
    switch (formula->get_type()) {
        case EF::XOR:
        case EF::OR:
        case EF::AND:
        case EF::IMPLIES:
            return std::make_shared<EF>(formula->get_type(), CongruenceAssistant::canonize_formula(formula->get_f1(), literal_to_CA), CongruenceAssistant::canonize_formula(formula->get_f2(), literal_to_CA));
        case EF::NOT:
            return std::make_shared<EF>(formula->get_type(), CongruenceAssistant::canonize_formula(formula->get_f1(), literal_to_CA));
        case EF::LITERAL:
            atom = CongruenceAtom::SPCA_from_variable(literal_to_CA, formula->get_literal());
            if (!CongruenceAtom::is_atom_variable(formula->get_literal()))
                return std::make_shared<EF>(formula->get_type(), formula->get_literal());
            atom = CongruenceAtom::SPCA_from_variable(literal_to_CA, formula->get_literal());
            return canonize_atom(atom, literal_to_CA, formula->get_literal());
        case EF::TRUE:
        case EF::FALSE:
            return std::make_shared<EF>(formula->get_type());
    }
    assert(false);
}


CongruenceAssistant::CongruenceAssistant(std::vector<SPCA> &literal_to_CA, std::shared_ptr<std::map<std::string, int>> name_to_variable, std::shared_ptr<satsolver::Formula> formula) :
    formula(formula), literal_to_CA(literal_to_CA), name_to_variable(*name_to_variable), variable_to_name(), adj_graph(), consistent_state(true), edge_of_cycle(std::make_pair(0, 0), 0), old_polarity(formula->get_nb_variables()+1, 0) {
    for (auto it : *name_to_variable)
        this->variable_to_name.insert(make_pair(it.second, it.first));
}

int CongruenceAssistant::on_flip(unsigned int variable) {
    unsigned int atom_id;
    SPCA atom;
    int clause_id=-1;
    std::pair<std::list<path_item>, int> r;
    if (VERBOSE_ASSISTANT && VERBOSE)
        std::cout << "flip variable: " << variable;
    if (!CongruenceAtom::is_atom_literal(this->variable_to_name, variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << ", which is not an atom." << std::endl;
        return true; // We care only about variables matching atoms.
    }
    assert(variable <= static_cast<unsigned int>(this->formula->get_nb_variables()));
    atom_id = CongruenceAtom::atom_id_from_literal(this->variable_to_name, variable);
    atom = CongruenceAtom::SPCA_from_literal(this->literal_to_CA, this->variable_to_name, variable);
    if (VERBOSE_ASSISTANT && VERBOSE)
        std::cout << ", whose atom is: " << atom_id << ", and whose new state is: ";
    if (this->formula->get_aff()->is_true(variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "true" << std::endl;
        assert(this->consistent_state);
        if (this->old_polarity[variable] == 1)
            return -1;
        this->old_polarity[variable] = 1;
    }
    else if (this->formula->get_aff()->is_false(variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "false" << std::endl;
        assert(this->consistent_state);
        if (this->old_polarity[variable] == -1)
            return -1;
        this->old_polarity[variable] = -1;
    }
    else {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "unknown" << std::endl;
        if (this->old_polarity[variable] == 0)
            return -1;
        this->old_polarity[variable] = 0;
    }
    return clause_id;
}

int CongruenceAssistant::literal_from_atom_id(int atom_id) const {
    if (atom_id > 0)
        return CongruenceAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(atom_id));
    else
        return -CongruenceAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(-atom_id));
}

/*
int CongruenceAssistant::learn_clause(std::list<path_item> &path, int atom_id) {
    std::unordered_set<int> clause;
    clause.insert(this->literal_from_atom_id(atom_id));
    for (auto it : path)
        clause.insert(this->literal_from_atom_id(it.tag));
    return static_cast<int>(this->formula->add_clause(std::make_shared<satsolver::Clause>(this->formula->get_nb_variables(), clause, this->formula->get_aff())));
}*/

bool CongruenceAssistant::is_state_consistent() {
    return this->consistent_state;
}

bool CongruenceAssistant::detect_isolated_literals() {
    return false;
}
