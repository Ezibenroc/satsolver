#ifndef STRUCTURES_EQUALITY_ASSISTANT_H
#define STRUCTURES_EQUALITY_ASSISTANT_H

#include <memory>

#include "structures/formula.h"
#include "structures/difference_atom.h"
#include "structures/extended_formula.h"
#include "solvers/abstract_assistant.h"

namespace theorysolver {

// An assistant for the equality theory
class DifferenceAssistant : public AbstractAssistant {
    private:
        std::shared_ptr<satsolver::Formula> formula;
        std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA;
    public:
        static std::shared_ptr<satsolver::ExtendedFormula> canonize_formula(std::shared_ptr<satsolver::ExtendedFormula> formula, std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA);
        DifferenceAssistant(std::vector<std::shared_ptr<DifferenceAtom>> &literal_to_DA, std::shared_ptr<satsolver::Formula> formula);

        bool on_flip(unsigned int variable);

        bool is_state_consistent();
};

}

#endif
