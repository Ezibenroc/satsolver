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

void StructuresTests::testBasicUsage() {
	std::vector <int> v = {1,2,-3,4,} ;
	Clause *c = new Clause(4,v) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({1,2,-3,4}));
	c->add(-2) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({1,-2,2,-3,4}));
	c->add(3) ;
	Clause *a = new Clause(*c) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({1,-2,2,-3,3,4}));
	c->remove(-1) ;c->remove(1) ; c->remove(2) ; c->remove(-2) ; c->remove(-3) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({3,4}));
	c->remove(3) ;	c->remove(4) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({}));
    CPPUNIT_ASSERT(a->to_set() == std::set<int>({1,-2,2,-3,3,4}));
}

CppUnit::Test* StructuresTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("StructuresTests");
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testClauseCreation",
                &StructuresTests::testClauseCreation));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testBasicUsage",
                &StructuresTests::testBasicUsage));
    return suite;
}
