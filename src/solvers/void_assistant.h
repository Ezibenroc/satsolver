#include "solvers/abstract_assistant.h"

namespace theorysolver {

/* An assistant that thinks everything is valid (as long as it is
 * valid for DPLL. Used when we don't want to have a theory. */
class VoidAssistant : public AbstractAssistant {
    public:
        int on_flip(unsigned int variable);
        virtual bool detect_isolated_literals();
        bool is_state_consistent();
        unsigned int get_depth_back() ;
};

}
