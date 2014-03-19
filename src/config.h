#ifndef CONFIG_H
#define CONFIG_H


namespace satsolver {
enum Heuristic {
    DUMB, RANDOM, MOMS, DLIS
};
}

extern bool VERBOSE;
extern bool WITH_WL;
extern satsolver::Heuristic HEURISTIC;

#endif
