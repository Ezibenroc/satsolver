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
    if(atom->left==atom->right) {
        switch (atom->op) {
            case EA::EQUAL: // t1 = t1   
                atom->canonical = SPEF(new EF(EF::TRUE));
                break;
            case EA::UNEQUAL: // t1 != t1  
                atom->canonical = SPEF(new EF(EF::FALSE));
                break;      
        } 
    }
    else {
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
    formula(formula), literal_to_EA(literal_to_EA), name_to_variable(*name_to_variable), variable_to_name(), union_find(), unequal(), consistent_state(true), depth_back(-1), old_polarity(formula->get_nb_variables()+1, 0) {
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
        return -1; // We care only about variables matching atoms.
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
        clause_id = this->insert_atom(atom->left, atom->right, atom->op == EA::EQUAL, atom_id, variable);
        this->old_polarity[variable] = 1;
    }
    else if (this->formula->get_aff()->is_false(variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "false" << std::endl;
        assert(this->consistent_state);
        if (this->old_polarity[variable] == -1)
            return -1;
        clause_id = this->insert_atom(atom->left, atom->right, atom->op == EA::UNEQUAL, atom_id, variable);
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
            assert(this->unequal.size());
            assert(this->unequal.back().first == atom_id);
            this->unequal.pop_back();
        }
        this->consistent_state = true;

        this->old_polarity[variable] = 0;
    }
    return clause_id;
}

int EqualityAssistant::insert_atom(unsigned int i, unsigned int j, bool equal, unsigned int atom_id, int lit_conf) {
    if (equal) {
        this->union_find.merge(atom_id, i, j);
        for (auto it : this->unequal) {
            if (this->union_find.find(it.second.first) == this->union_find.find(it.second.second)) {
                this->consistent_state = false;
                return this->learn_clause(it.first, i, j, lit_conf);
            }
        }
    }
    else {
        this->consistent_state = (this->union_find.find(i) != this->union_find.find(j));
        this->unequal.push_back(std::make_pair(atom_id, std::make_pair(i, j)));
        if (!this->consistent_state)
            return this->learn_clause(atom_id, i, j, lit_conf);
    }
    return -1;
}

int EqualityAssistant::literal_from_atom_id(int atom_id) const {
    if (atom_id > 0)
        return EqualityAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(atom_id));
    else
        return -EqualityAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(-atom_id));
}

#define invert_polarity(lit) this->formula->get_aff()->is_true(lit) ? -lit : lit

int EqualityAssistant::learn_clause(int unequal_atomid, int i, int j, int lit_conf) {
    std::unordered_set<int> clause;
    int max_depth=-1, max_depth_l=0 ;
    int lit;
    int tmp;
    lit_conf = invert_polarity(lit_conf) ;
    clause.insert(lit_conf);
    clause.insert(lit=invert_polarity(this->literal_from_atom_id(unequal_atomid)));
    if(lit!=lit_conf) {
        max_depth = this->formula->get_ded()->get_deduction_depth(lit) ;
        max_depth_l = lit ;
    }
    for (auto it : this->union_find.get_path(i)) {
        lit = invert_polarity(this->literal_from_atom_id(it));
        clause.insert(lit);
        if(lit!=lit_conf && this->formula->get_ded()->get_deduction_depth(lit) > max_depth) {
            max_depth = this->formula->get_ded()->get_deduction_depth(lit) ;
            max_depth_l = lit ;
        }
    } 
    for (auto it : this->union_find.get_path(j)) {
        lit = invert_polarity(this->literal_from_atom_id(it));
        clause.insert(lit);
        if(lit!=lit_conf && this->formula->get_ded()->get_deduction_depth(lit) > max_depth) {
            max_depth = this->formula->get_ded()->get_deduction_depth(lit) ;
            max_depth_l = lit ;
        }
    }
    assert(max_depth >= 0);
    this->depth_back = max_depth ;
    assert(clause.size()>=2) ;
    assert(this->formula->get_aff());
    tmp = static_cast<int>(this->formula->add_clause(std::make_shared<satsolver::Clause>(this->formula->get_nb_variables(), clause, this->formula->get_aff())));
    if(WITH_WL) this->formula->to_clauses_vector()[tmp]->init_WL_CL(lit_conf,max_depth_l) ;
    return tmp ;
}

bool EqualityAssistant::is_state_consistent() {
    return this->consistent_state;
}

bool EqualityAssistant::detect_isolated_literals() {
    return false;
}

unsigned int EqualityAssistant::get_depth_back() {
    assert(this->depth_back>=0) ;
    unsigned int tmp = static_cast<unsigned int>(this->depth_back) ; // cette fonction ne doit être appelée qu'une fois par clause apprise
    this->depth_back = -1 ;
    return tmp ;
}

unsigned int EqualityAssistant::get_component(unsigned int i) const {
    return this->union_find.find(i);
}
unsigned int EqualityAssistant::get_max_node() const {
    return this->union_find.get_max_node();
}
