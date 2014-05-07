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
            id1 = DifferenceAtom::add_DA(literal_to_DA, atom->i, atom->j, DA::LEQ, atom->n-1);
            atom->canonical = SPEF(new EF(EF::LITERAL, DifferenceAtom::variable_name_from_atom_id(id1)));
            break;
        case DA::LEQ: // xi - xj <= n   -->   xi - xj <= n
            atom->canonical = SPEF(new EF(EF::LITERAL, literal));
            break;
        case DA::GREATER: // xi - xj > n   -->   ~(xi - xj <= n)
            id1 = DifferenceAtom::add_DA(literal_to_DA, atom->i, atom->j, DA::LEQ, atom->n);
            atom->canonical = SPEF(new EF(EF::NOT,
                        SPEF(new EF(EF::LITERAL, DifferenceAtom::variable_name_from_atom_id(id1)))
                   ));
            break;
        case DA::GEQ: // xi - xj >= n   -->   xj - xi <= -n
            id1 = DifferenceAtom::add_DA(literal_to_DA, atom->j, atom->i, DA::LEQ, -atom->n);
            atom->canonical = SPEF(new EF(EF::LITERAL, DifferenceAtom::variable_name_from_atom_id(id1)));
            break;
        case DA::EQUAL: // xi - xj = n   -->   xi - xj <= n  ∧  xj - xi <= -n
            id1 = DifferenceAtom::add_DA(literal_to_DA, atom->i, atom->j, DA::LEQ, atom->n);
            id2 = DifferenceAtom::add_DA(literal_to_DA, atom->j, atom->i, DA::LEQ, -atom->n);
            atom->canonical = SPEF(new EF(EF::AND,
                SPEF(new EF(EF::LITERAL, DifferenceAtom::variable_name_from_atom_id(id1))),
                SPEF(new EF(EF::LITERAL, DifferenceAtom::variable_name_from_atom_id(id2)))));
            break;
        case DA::UNEQUAL: // xi - xj != n   -->   xi - xj <= n-1  ∨  xj - xi <= -n-1
            id1 = DifferenceAtom::add_DA(literal_to_DA, atom->i, atom->j, DA::LEQ, atom->n-1);
            id2 = DifferenceAtom::add_DA(literal_to_DA, atom->j, atom->i, DA::LEQ, -atom->n-1);
            atom->canonical = SPEF(new EF(EF::OR,
                SPEF(new EF(EF::LITERAL, DifferenceAtom::variable_name_from_atom_id(id1))),
                SPEF(new EF(EF::LITERAL, DifferenceAtom::variable_name_from_atom_id(id2)))));
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
            atom = DifferenceAtom::SPDA_from_variable(literal_to_DA, formula->get_literal());
            if (!DifferenceAtom::is_atom_variable(formula->get_literal()))
                return std::make_shared<EF>(formula->get_type(), formula->get_literal());
            atom = DifferenceAtom::SPDA_from_variable(literal_to_DA, formula->get_literal());
            return canonize_atom(atom, literal_to_DA, formula->get_literal());
        case EF::TRUE:
        case EF::FALSE:
            return std::make_shared<EF>(formula->get_type());
    }
    assert(false);
}


DifferenceAssistant::DifferenceAssistant(std::vector<SPDA> &literal_to_DA, std::shared_ptr<std::map<std::string, int>> name_to_variable, std::shared_ptr<satsolver::Formula> formula) :
    formula(formula), literal_to_DA(literal_to_DA), name_to_variable(*name_to_variable), variable_to_name(), adj_graph(), consistent_state(true), edge_of_cycle(0, 0) {
    for (auto it : *name_to_variable)
        this->variable_to_name.insert(make_pair(it.second, it.first));
}

bool DifferenceAssistant::on_flip(unsigned int variable) {
    unsigned int atom_id;
    SPDA atom;
    unsigned int i, j;
    int n;
    std::pair<std::list<std::pair<unsigned int, int>>, int> r;
    if (!DifferenceAtom::is_atom_literal(this->variable_to_name, variable)) {
        return true; // We care only about variables matching atoms.
    }
    atom_id = DifferenceAtom::atom_id_from_literal(this->variable_to_name, variable);
    atom = DifferenceAtom::SPDA_from_literal(this->literal_to_DA, this->variable_to_name, variable);
    i = atom->i;
    j = atom->j;
    n = atom->n;
    if (this->formula->get_aff()->is_true(variable)) {
        r = this->adj_graph.find_lowest_path(j, i);
        this->adj_graph.add_edge(i, j, atom_id, n);
        if (r.second < n) { // It creates a negative cycle
            this->learn_clause(r.first, atom_id);
            assert(this->consistent_state);
            this->edge_of_cycle = std::make_pair(i, j);
            this->consistent_state = false;
        }
        this->adj_graph.delete_edge(j, i);
    }
    else if (this->formula->get_aff()->is_false(variable)) {
        r = this->adj_graph.find_lowest_path(i, j);
        this->adj_graph.add_edge(j, i, -atom_id, -n-1);
        if (r.second < n) { // It creates a negative cycle
            this->learn_clause(r.first, -atom_id);
            assert(this->consistent_state);
            this->edge_of_cycle = std::make_pair(j, i);
            this->consistent_state = false;
        }
        // ~(xi - xj <= n) <-> (xj - xi <= -n-1)
        this->adj_graph.delete_edge(i, j);
    }
    else {
        this->adj_graph.delete_edge(i, j);
        if (!this->consistent_state) {
            if (i == this->edge_of_cycle.first && j == this->edge_of_cycle.second)
                this->consistent_state = true;
            else {
                r = this->adj_graph.find_lowest_path(this->edge_of_cycle.second, this->edge_of_cycle.first);
                // u->v was part of all negative cycles of the graph.
                // Search if there is no more negative cycle containing u->v.
                if (r.second >= this->adj_graph.get_weight(this->edge_of_cycle.first, this->edge_of_cycle.second)) {
                    this->consistent_state = false;
                }
            }

        }
    }
    return this->consistent_state;
}

int DifferenceAssistant::literal_from_atom_id(int atom_id) const {
    if (atom_id > 0)
        return DifferenceAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(atom_id));
    else
        return -DifferenceAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(-atom_id));
}

void DifferenceAssistant::learn_clause(std::list<std::pair<unsigned int, int>> &path, int atom_id) {
    std::unordered_set<int> clause;
    clause.insert(this->literal_from_atom_id(atom_id));
    for (auto it : path)
        clause.insert(this->literal_from_atom_id(it.second));
    this->formula->add_clause(std::make_shared<satsolver::Clause>(this->formula->get_nb_variables(), clause));
}

bool DifferenceAssistant::is_state_consistent() {
    return this->consistent_state;
}
