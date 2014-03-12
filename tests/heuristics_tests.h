#ifndef HEURISTICS_TESTS_H
#define HEURISTICS_TESTS_H

#include <cppunit/TestFixture.h>
class HeuristicsTests : public CppUnit::TestFixture {
    private:
        void testRandom();
        void testMoms() ;

    public:
        static CppUnit::Test* suite();
};

#endif
