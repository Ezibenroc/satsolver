#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include <vector>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "structures/clause.h"
#include "structures/formula.h"
#include "structures/affectation.h"
#include "dpll.h"
#include "dpll_tests.h"

using namespace satsolver;


void DpllTests::testBasicUsage() {
    Clause *c1=new Clause(3, {1, 2, 3}), *c2=new Clause(3, {-1, 2, 3}),
           *c3=new Clause(3, {1, -2, 3}), *c4=new Clause(3, {-1, -2, 3}),
           *c5=new Clause(3, {1, 2, -3}), *c6=new Clause(3, {-1, 2, -3}),
           *c7=new Clause(3, {1, -2, -3}), *c8=new Clause(3, {-1, -2, -3});
    std::vector<Clause*> clauses;
    Formula *f;
    Affectation *solution;
    clauses.push_back(c1);
    clauses.push_back(c2);
    clauses.push_back(c3);
    clauses.push_back(c4);
    clauses.push_back(c5);
    clauses.push_back(c6);
    clauses.push_back(c8);
    f = new Formula(clauses, 3);
    solution = solve(f);
    CPPUNIT_ASSERT(solution->is_false(1));
    CPPUNIT_ASSERT(solution->is_true(2));
    CPPUNIT_ASSERT(solution->is_true(3));

    clauses.push_back(c7);
    f = new Formula(clauses, 3);
    CPPUNIT_ASSERT_THROW(solve(f), Conflict);
   
}

CppUnit::Test* DpllTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("DpllTests");
    suite->addTest(new CppUnit::TestCaller<DpllTests>("testBasicUsage",
                &DpllTests::testBasicUsage));
    return suite;
}

