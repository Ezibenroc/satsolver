#include <cppunit/ui/text/TestRunner.h>
#include "structures_tests.h"
#include "parser_tests.h"
#include "dpll_tests.h"

bool VERBOSE = false;
bool WITH_WL = false;

int main(void)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(StructuresTests::suite());
   	runner.addTest(ParserTests::suite());
    runner.addTest(DpllTests::suite());
    runner.run();
    return 0;
}
