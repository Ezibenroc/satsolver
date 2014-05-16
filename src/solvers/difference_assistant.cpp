#include <cassert>
#include <iostream>
#include <climits>
#include "solvers/difference_assistant.h"
#include "config.h"

using namespace theorysolver;

#define VERBOSE_ASSISTANT false

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
    if (atom->i == atom->j) {
        bool v;
        switch (atom->op) {
            case DA::LOWER:
                v = atom->n > 0;
                break;
            case DA::LEQ:
                v = atom->n >= 0;
                break;
            case DA::GREATER:
                v = atom->n < 0;
                break;
            case DA::GEQ:
                v = atom->n <= 0;
                break;
            case DA::EQUAL:
                v = atom->n == 0;
                break;
            case DA::UNEQUAL:
                v = atom->n != 0;
                break;
            default:
                assert(false);
        }
        atom->canonical = SPEF(new EF(v ? EF::TRUE : EF::FALSE));
        return atom->canonical;
    }
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
    formula(formula), literal_to_DA(literal_to_DA), name_to_variable(*name_to_variable), variable_to_name(), adj_graph(), consistent_state(true), edge_of_cycle(std::make_pair(0, 0), 0), old_polarity(formula->get_nb_variables()+1, 0) {
    for (auto it : *name_to_variable)
        this->variable_to_name.insert(make_pair(it.second, it.first));
}

int DifferenceAssistant::on_flip(unsigned int variable) {
    unsigned int atom_id;
    SPDA atom;
    unsigned int i, j, tmp;
    int clause_id=-1;
    int n;
    std::pair<std::list<path_item>, int> r;
    if (VERBOSE_ASSISTANT && VERBOSE)
        std::cout << "flip variable: " << variable;
    if (!DifferenceAtom::is_atom_literal(this->variable_to_name, variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << ", which is not an atom." << std::endl;
        return -1; // We care only about variables matching atoms.
    }
    assert(variable <= static_cast<unsigned int>(this->formula->get_nb_variables()));
    atom_id = DifferenceAtom::atom_id_from_literal(this->variable_to_name, variable);
    atom = DifferenceAtom::SPDA_from_literal(this->literal_to_DA, this->variable_to_name, variable);
    if (VERBOSE_ASSISTANT && VERBOSE)
        std::cout << ", whose atom is: " << atom_id << ", and whose new state is: ";
    i = atom->i;
    j = atom->j;
    n = atom->n;
    if (this->formula->get_aff()->is_true(variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "true" << std::endl;
        assert(this->consistent_state);
        if (this->old_polarity[variable] == 1)
            return -1;
        r = this->adj_graph.find_lowest_path(j, i);
        this->adj_graph.add_edge(i, j, atom_id, n);
        if (r.second < -n) { // It creates a negative cycle
            clause_id = this->learn_clause(r.first, atom_id);
            assert(this->consistent_state);
            this->edge_of_cycle = std::make_pair(std::make_pair(i, j), variable);
            this->consistent_state = false;
        }
        this->old_polarity[variable] = 1;
    }
    else if (this->formula->get_aff()->is_false(variable)) {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "false" << std::endl;
        assert(this->consistent_state);
        if (this->old_polarity[variable] == -1)
            return -1;
        r = this->adj_graph.find_lowest_path(i, j);
        this->adj_graph.add_edge(j, i, atom_id, -n-1);
        if (r.second < n+1) { // It creates a negative cycle
            clause_id = this->learn_clause(r.first, atom_id);
            assert(this->consistent_state);
            this->edge_of_cycle = std::make_pair(std::make_pair(j, i), variable);
            this->consistent_state = false;
        }
        this->old_polarity[variable] = -1;
        // ~(xi - xj <= n) <-> (xj - xi <= -n-1)
    }
    else {
        if (VERBOSE_ASSISTANT && VERBOSE)
            std::cout << "unknown" << std::endl;
        if (this->old_polarity[variable] == 0)
            return -1;
        if (this->old_polarity[variable] == -1) {
            tmp = i;
            i = j;
            j = tmp;
        }
        this->adj_graph.delete_edge(i, j, atom_id);
        if (!this->consistent_state) {
            if (i == this->edge_of_cycle.first.first && j == this->edge_of_cycle.first.second)
                this->consistent_state = true;
            else {
                r = this->adj_graph.find_lowest_path(this->edge_of_cycle.first.second, this->edge_of_cycle.first.first);
                // u->v was part of all negative cycles of the graph.
                // Search if there is no more negative cycle containing u->v.
                if (r.second >= -this->adj_graph.get_weight(this->edge_of_cycle.second)) {
                    this->consistent_state = true;
                }
            }

        }
        this->old_polarity[variable] = 0;
    }
    return clause_id;
}

int DifferenceAssistant::literal_from_atom_id(int atom_id) const {
    if (atom_id > 0)
        return DifferenceAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(atom_id));
    else
        return -DifferenceAtom::literal_from_atom_id(this->name_to_variable, static_cast<unsigned int>(-atom_id));
}

int DifferenceAssistant::learn_clause(std::list<path_item> &path, int atom_id) {
    int max_depth=-1, max_depth_l=0 ;
    int lit, lit_conf = -this->literal_from_atom_id(atom_id) ;
    int tmp ;
    std::unordered_set<int> clause;
    clause.insert(lit_conf);
    for (auto it : path) {
        clause.insert(lit=-this->literal_from_atom_id(it.tag));
        assert(this->formula->get_aff()->is_false(lit)) ; // tous les littéraux de la clause apprises doivent être faux
        if(WITH_WL && lit!=lit_conf && this->formula->get_ded()->get_deduction_depth(lit) > max_depth) {
            max_depth = this->formula->get_ded()->get_deduction_depth(lit) ;
            max_depth_l = lit ;
        }
    }
    assert(clause.size()>=2) ;
    assert(!WITH_WL || max_depth_l) ;
    tmp = static_cast<int>(this->formula->add_clause(std::make_shared<satsolver::Clause>(this->formula->get_nb_variables(), clause, this->formula->get_aff())));
    if(WITH_WL) this->formula->to_clauses_vector()[tmp]->init_WL_CL(lit_conf,max_depth_l) ;
    return tmp ;
}

bool DifferenceAssistant::is_state_consistent() {
    return this->consistent_state;
}

bool DifferenceAssistant::detect_isolated_literals() {
    return false;
}
