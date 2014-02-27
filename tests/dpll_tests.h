#ifndef DPLL_TESTS_H
#define DPLL_TESTS_H

#include <cppunit/TestFixture.h>
class DpllTests : public CppUnit::TestFixture {
    private:
        void testBasicUsageWithWL();
        void testBasicUsageWithoutWL();
    public:
        static CppUnit::Test* suite();
};

#endif

