#ifndef THEORY_TESTS_H
#define THEORY_TESTS_H

#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>

class TheoryTests : public CppUnit::TestFixture {
    private:
        void testDifferencesObviousFail();
        void testDifferencesLessObviousFail();
    public:
        static CppUnit::Test* suite();
};

#endif

