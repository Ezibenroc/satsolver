#include "solvers/abstract_assistant.h"

namespace theorysolver {

class VoidAssistant : public AbstractAssistant {
    public:
        bool on_flip(unsigned int variable);
        bool is_state_consistent();
};

}
