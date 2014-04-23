#ifndef DPLL_TESTS_H
#define DPLL_TESTS_H

#include <cppunit/TestFixture.h>
class DpllTests : public CppUnit::TestFixture {
    private:
        void testBasicUsage();
        void testBasicUsageWithWL();
        void testBasicUsageWithCL();
        void testBasicUsageWithWLWithCL();
        void testSmallConflict();
        void testSmallConflictWithWL();
        void testSmallConflictWithCL();
        void testSmallConflictWithWLWithCL();
        void testRAND() ;
        void testMOMS() ;
        void testDLIS() ;
    public:
        static CppUnit::Test* suite();
};

#endif

