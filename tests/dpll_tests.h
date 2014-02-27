#ifndef DPLL_TESTS_H
#define DPLL_TESTS_H

#include <cppunit/TestFixture.h>
class DpllTests : public CppUnit::TestFixture {
    private:
        void testBasicUsage();
        void testBasicUsageWithWL();
        void testSmallConflict();
        void testSmallConflictWithWL();
    public:
        static CppUnit::Test* suite();
};

#endif

