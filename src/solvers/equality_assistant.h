#ifndef STRUCTURES_CONGRUENCE_ASSISTANT_H
#define STRUCTURES_CONGRUENCE_ASSISTANT_H

#include <memory>
#include <map>

#include "structures/formula.h"
#include "structures/equality_atom.h"
#include "structures/extended_formula.h"
#include "structures/union_find.h"
#include "solvers/abstract_assistant.h"

namespace theorysolver {

// An assistant for the congruene and equality theories
class EqualityAssistant : public AbstractAssistant {
    private:
        std::shared_ptr<satsolver::Formula> formula;
        std::vector<std::shared_ptr<EqualityAtom>> &literal_to_EA;
        std::map<std::string, int> name_to_variable;
        std::map<int, std::string> variable_to_name;
        UnionFind union_find;
        std::vector<std::pair<unsigned int, unsigned int>> unequal;
        bool consistent_state;
        // As we forbid adding an edge when there is a negative cycle,
        // the following edge belong to *all* possible negative cycles
        // of the graphe.
        std::pair<std::pair<unsigned int, unsigned int>, unsigned int> edge_of_cycle; // ((u, v), tag)
        std::vector<int> old_polarity;

        int literal_from_atom_id(int atom_id) const;
        void insert_atom(unsigned int i, unsigned int j, bool equal);
    public:
        static std::shared_ptr<satsolver::ExtendedFormula> canonize_formula(std::shared_ptr<satsolver::ExtendedFormula> formula, std::vector<std::shared_ptr<EqualityAtom>> &literal_to_EA);
        EqualityAssistant(std::vector<std::shared_ptr<EqualityAtom>> &literal_to_EA, std::shared_ptr<std::map<std::string, int>> name_to_variable, std::shared_ptr<satsolver::Formula> formula);

        /* Return the index of the learned clause, if any. -1 otherwise. */
        int on_flip(unsigned int variable);

        bool is_state_consistent();
        bool detect_isolated_literals();

        unsigned int get_depth_back();
};

}

#endif
