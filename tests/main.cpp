#include <cppunit/ui/text/TestRunner.h>
#include "structures_tests.h"
#include "satparser_tests.h"
#include "graphparser_tests.h"
#include "dpll_tests.h"

bool VERBOSE = false;
bool WITH_WL = false;

int main(void)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(StructuresTests::suite());
    runner.addTest(DpllTests::suite());
    runner.addTest(SatParserTests::suite());
    runner.addTest(GraphParserTests::suite());
    runner.run();
    return 0;
}
