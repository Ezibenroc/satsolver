#include "solvers/difference_assistant.h"

using namespace theorysolver;

#define DA DifferenceAtom
#define SPDA std::shared_ptr<DA>


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
