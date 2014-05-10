#include "solvers/abstract_assistant.h"

namespace theorysolver {

/* An assistant that thinks everything is valid (as long as it is
 * valid for DPLL. Used when we don't want to have a theory. */
class VoidAssistant : public AbstractAssistant {
    public:
        unsigned int on_flip(unsigned int variable);
        bool is_state_consistent();
};

}
