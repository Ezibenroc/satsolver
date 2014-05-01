#ifndef STRUCTURES_ABSTRACT_ASSISTANT_H
#define STRUCTURES_ABSTRACT_ASSISTANT_H

namespace theorysolver {

class AbstractAssistant {
    public:
        // Called every time we set a variable to true or to false.
        // Returns true iff the new state is consistent.
        virtual bool on_flip(unsigned int variable) = 0;

        virtual bool is_state_consistent() = 0;
};

}

#endif
