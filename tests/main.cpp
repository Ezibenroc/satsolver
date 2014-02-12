#include <cppunit/ui/text/TestRunner.h>
#include "structures_tests.h"

int main(void)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(StructuresTests::suite());
    runner.run();
    return 0;
}
