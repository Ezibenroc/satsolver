#include <cassert>
#include <iostream>
#include <climits>
#include "solvers/equality_assistant.h"
#include "config.h"

using namespace theorysolver;

#define VERBOSE_ASSISTANT false

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define EA EqualityAtom
#define SPEA std::shared_ptr<EA>

// Return a formula that uses only atoms in the canonical “xi-xj<=n” form.
// Note that it may use literal #0; for instance, xi >= n is converted to
// x0 - xi <= -n, so we will have to make sure x0 is always equal to 0.
SPEF canonize_atom(SPEA atom, std::vector<SPEA> &literal_to_EA, std::string literal) {
    unsigned long int id1;
    assert(atom);
    switch (atom->op) {
        case EA::EQUAL: // t1 = t2   --> t1 = t2
            atom->canonical = SPEF(new EF(EF::LITERAL, literal));
            break;
        case EA::UNEQUAL: // t2 != t2   -->   ~(t1 = t2)
            id1 = EqualityAtom::add_EA(literal_to_EA, atom->left, EA::EQUAL, atom->right);
            atom->canonical = SPEF(new EF(EF::NOT,
                        SPEF(new EF(EF::LITERAL, EqualityAtom::variable_name_from_atom_id(id1)))
                   ));
            break;
    }
    return atom->canonical;
}

SPEF EqualityAssistant::canonize_formula(SPEF formula, std::vector<SPEA> &literal_to_EA) {
    SPEA atom;
    switch (formula->get_type()) {
        case EF::XOR:
        case EF::OR:
        case EF::AND:
        case EF::IMPLIES:
            return std::make_shared<EF>(formula->get_type(), EqualityAssistant::canonize_formula(formula->get_f1(), literal_to_EA), EqualityAssistant::canonize_formula(formula->get_f2(), literal_to_EA));
        case EF::NOT:
            return std::make_shared<EF>(formula->get_type(), EqualityAssistant::canonize_formula(formula->get_f1(), literal_to_EA));
        case EF::LITERAL:
            atom = EqualityAtom::SPEA_from_variable(literal_to_EA, formula->get_literal());
            if (!EqualityAtom::is_atom_variable(formula->get_literal()))
                return std::make_shared<EF>(formula->get_type(), formula->get_literal());
            atom = EqualityAtom::SPEA_from_variable(literal_to_EA, formula->get_literal());
            return canonize_atom(atom, literal_to_EA, formula->get_literal());
        case EF::TRUE:
        case EF::FALSE:
            return std::make_shared<EF>(formula->get_type());
    }
    assert(false);
}


EqualityAssistant::EqualityAssistant(std::vector<SPEA> &literal_to_EA, std::shared_ptr<std::map<std::string, int>> name_to_variable, std::shared_ptr<satsolver::Formula> formula) :
    formula(formula), literal_to_EA(literal_to_EA), name_to_variable(*name_to_variable), variable_to_name(), union_find(), unequal(), consistent_state(true), edge_of_cycle(std::make_pair(0, 0), 0), old_polarity(formula->get_nb_variables()+1, 0) {
    for (auto it : *name_to_variable)
        this->variable_to_name.insert(make_pair(it.second, it.first));
}

int EqualityAssistant::on_flip(unsigned int variable) {
    unsigned int atom_id;
    SPEA atom;
    int clause_id=-1;
    if (VERBOSE_ASSISTANT && VERBOSE)
        std::cout << "flip variable: " << variable;
    if (!EqualityAtom::is_atom_literal(this->variable_to_name, variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << ", which is not an atom." << std::endl;
        return true; // We care only about variables matching atoms.
    }
    assert(variable <= static_cast<unsigned int>(this->formula->get_nb_variables()));
    atom_id = EqualityAtom::atom_id_from_literal(this->variable_to_name, variable);
    atom = EqualityAtom::SPEA_from_literal(this->literal_to_EA, this->variable_to_name, variable);
    if (VERBOSE_ASSISTANT && VERBOSE)
        std::cout << ", whose atom is: " << atom_id << ", and whose new state is: ";
    if (this->formula->get_aff()->is_true(variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "true" << std::endl;
        assert(this->consistent_state);
        if (this->old_polarity[variable] == 1)
            return -1;
        this->insert_atom(atom->left, atom->right, atom->op == EA::EQUAL);
        this->old_polarity[variable] = 1;
    }
    else if (this->formula->get_aff()->is_false(variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "false" << std::endl;
        assert(this->consistent_state);
        if (this->old_polarity[variable] == -1)
            return -1;
        this->insert_atom(atom->left, atom->right, atom->op == EA::UNEQUAL);
        this->old_polarity[variable] = -1;
    }
    else {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "unknown" << std::endl;
        if (this->old_polarity[variable] == 0)
            return -1;
        if (this->old_polarity[variable] == 1) {
            this->union_find.unmerge();
        }
        else {
            this->unequal.pop_back();
        }

        this->old_polarity[variable] = 0;
    }
    return clause_id;
}

void EqualityAssistant::insert_atom(unsigned int i, unsigned int j, bool equal) {
    if (equal) {
        this->union_find.merge(i, j);
        for (auto it : this->unequal) {
            if (this->union_find.find(it.first) == this->union_find.find(it.first)) {
                this->consistent_state = false;
                break;
            }
        }
    }
    else {
        this->consistent_state = (this->union_find.find(i) != this->union_find.find(j));
        this->unequal.push_back(std::make_pair(i, j));
    }
}

int EqualityAssistant::literal_from_atom_id(int atom_id) const {
    if (atom_id > 0)
        return EqualityAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(atom_id));
    else
        return -EqualityAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(-atom_id));
}

/*
int EqualityAssistant::learn_clause(std::list<path_item> &path, int atom_id) {
    std::unordered_set<int> clause;
    clause.insert(this->literal_from_atom_id(atom_id));
    for (auto it : path)
        clause.insert(this->literal_from_atom_id(it.tag));
    return static_cast<int>(this->formula->add_clause(std::make_shared<satsolver::Clause>(this->formula->get_nb_variables(), clause, this->formula->get_aff())));
}*/

bool EqualityAssistant::is_state_consistent() {
    return this->consistent_state;
}

bool EqualityAssistant::detect_isolated_literals() {
    return false;
}

unsigned int EqualityAssistant::get_depth_back() {
    return -1;
}
