#ifndef GRAPHSOLVER_TESTS_H
#define GRAPHSOLVER_TESTS_H

#include <cppunit/TestFixture.h>

class GraphSolverTests : public CppUnit::TestFixture {
    private:
        void testEdgeFormula();
        void testMaxFormula();
        void testSolve();
        void testSolveExtremeCases();
    public:
        static CppUnit::Test* suite();
};

#endif

