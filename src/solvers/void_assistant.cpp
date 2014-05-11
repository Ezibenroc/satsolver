#include "solvers/void_assistant.h"

using namespace theorysolver;


int VoidAssistant::on_flip(unsigned int variable __attribute__((unused))) {
    return -1;
}
bool VoidAssistant::is_state_consistent() {
    return true;
}
