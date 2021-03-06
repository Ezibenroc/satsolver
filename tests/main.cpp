#include <cppunit/ui/text/TestRunner.h>
#include "structures_tests.h"
#include "satparser_tests.h"
#include "graphparser_tests.h"
#include "graphsolver_tests.h"
#include "dpll_tests.h"
#include "theory_tests.h"
#include "heuristics_tests.h"
#include "structures/formula.h"
#include "config.h"
#include <time.h>
#include <stdlib.h>

bool VERBOSE = false;
bool WITH_WL = false;
bool CL_INTERACT = false;
bool WITH_CL = false;
bool CL_STATS = false;
satsolver::Heuristic HEURISTIC = satsolver::DUMB ;

int main(void) {
    srand(static_cast<unsigned int>(time(NULL)));
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(StructuresTests::suite());
    runner.addTest(HeuristicsTests::suite());
    runner.addTest(DpllTests::suite());
    runner.addTest(SatParserTests::suite());
    runner.addTest(GraphParserTests::suite());
    runner.addTest(GraphSolverTests::suite());
    runner.addTest(TheoryTests::suite());
    runner.run();
    return 0;
}
