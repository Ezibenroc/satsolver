#include "solvers/void_assistant.h"
#include <cassert>

using namespace theorysolver;


int VoidAssistant::on_flip(unsigned int variable __attribute__((unused))) {
    return -1;
}
bool VoidAssistant::detect_isolated_literals() {
    return true;
}
bool VoidAssistant::is_state_consistent() {
    return true;
}

unsigned int VoidAssistant::get_depth_back() {
    assert(false) ;
    return 0 ;
}
