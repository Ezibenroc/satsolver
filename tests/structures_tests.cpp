#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include "structures.cpp"
#include "structures_tests.h"

void StructuresTests::testClauseCreation() {
    std::vector<int> v(2);
    v = {3, -2};
    satsolver::Clause clause(5, v);
    CPPUNIT_ASSERT(!clause.contains_literal(-5));
    CPPUNIT_ASSERT(!clause.contains_literal(-4));
    CPPUNIT_ASSERT(!clause.contains_literal(-3));
    CPPUNIT_ASSERT(clause.contains_literal(-2));
    CPPUNIT_ASSERT(!clause.contains_literal(-1));
    CPPUNIT_ASSERT(!clause.contains_literal(1));
    CPPUNIT_ASSERT(!clause.contains_literal(2));
    CPPUNIT_ASSERT(clause.contains_literal(3));
    CPPUNIT_ASSERT(!clause.contains_literal(4));
    CPPUNIT_ASSERT(!clause.contains_literal(5));
}

CppUnit::Test* StructuresTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("StructuresTests");
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testClauseCreation",
                &StructuresTests::testClauseCreation));
    return suite;
}
