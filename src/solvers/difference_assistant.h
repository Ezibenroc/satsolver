#ifndef STRUCTURES_DIFFERENCE_ASSISTANT_H
#define STRUCTURES_DIFFERENCE_ASSISTANT_H

#include <memory>
#include <map>

#include "structures/formula.h"
#include "structures/difference_atom.h"
#include "structures/extended_formula.h"
#include "structures/adj_graph.h"
#include "solvers/abstract_assistant.h"

namespace theorysolver {

// An assistant for the difference theory
class DifferenceAssistant : public AbstractAssistant {
    private:
        std::shared_ptr<satsolver::Formula> formula;
        std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA;
        std::map<std::string, int> name_to_variable;
        std::map<int, std::string> variable_to_name;
        AdjGraph adj_graph;
        bool consistent_state;
        // As we forbid adding an edge when there is a negative cycle,
        // the following edge belong to *all* possible negative cycles
        // of the graphe.
        std::pair<std::pair<unsigned int, unsigned int>, unsigned int> edge_of_cycle; // ((u, v), tag)
        std::vector<int> old_polarity;
        int depth_back ; // niveau du backtrack éventuel à faire
        int literal_from_atom_id(int atom_id) const;
        
    public:
        static std::shared_ptr<satsolver::ExtendedFormula> canonize_formula(std::shared_ptr<satsolver::ExtendedFormula> formula, std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA);
        DifferenceAssistant(std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA, std::shared_ptr<std::map<std::string, int>> name_to_variable, std::shared_ptr<satsolver::Formula> formula);

        /* Return the index of the learned clause, if any. -1 otherwise. */
        int on_flip(unsigned int variable);
        int learn_clause(std::list<path_item> &path, int atom_id);

        bool is_state_consistent();
        bool detect_isolated_literals();

        const AdjGraph& get_graph() { return this->adj_graph; }
        
        unsigned int get_depth_back() ;
};

}

#endif
