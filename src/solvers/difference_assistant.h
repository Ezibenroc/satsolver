#ifndef STRUCTURES_EQUALITY_ASSISTANT_H
#define STRUCTURES_EQUALITY_ASSISTANT_H

#include <memory>
#include <map>

#include "structures/formula.h"
#include "structures/difference_atom.h"
#include "structures/extended_formula.h"
#include "structures/adj_graph.h"
#include "solvers/abstract_assistant.h"

namespace theorysolver {

// An assistant for the equality theory
class DifferenceAssistant : public AbstractAssistant {
    private:
        std::shared_ptr<satsolver::Formula> formula;
        std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA;
        std::map<std::string, int> name_to_variable;
        std::map<int, std::string> variable_to_name;
        AdjGraph adj_graph;

        int literal_from_atom_id(int atom_id) const;
    public:
        static std::shared_ptr<satsolver::ExtendedFormula> canonize_formula(std::shared_ptr<satsolver::ExtendedFormula> formula, std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA);
        DifferenceAssistant(std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA, std::shared_ptr<std::map<std::string, int>> name_to_variable, std::shared_ptr<satsolver::Formula> formula);

        bool on_flip(unsigned int variable);
        void learn_clause(std::list<std::pair<unsigned int, int>> &path, int atom_id);

        bool is_state_consistent();
};

}

#endif
