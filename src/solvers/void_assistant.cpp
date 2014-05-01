#include "solvers/void_assistant.h"

using namespace theorysolver;


bool VoidAssistant::on_flip(unsigned int variable __attribute__((unused))) {
    return true;
}
bool VoidAssistant::is_state_consistent() {
    return true;
}
