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


void get_test_clauses(
            std::vector<std::shared_ptr<Clause>> *clauses,
            std::vector<std::shared_ptr<Clause>> *clauses2) {
    Clause *c1=new Clause(3, {1, 2, 3}), *c2=new Clause(3, {-1, 2, 3}),
           *c3=new Clause(3, {1, -2, 3}), *c4=new Clause(3, {-1, -2, 3}),
           *c5=new Clause(3, {1, 2, -3}), *c6=new Clause(3, {-1, 2, -3}),
           *c7=new Clause(3, {1, -2, -3}), *c8=new Clause(3, {-1, -2, -3});
    (*clauses).push_back(std::shared_ptr<Clause>(c1));
    (*clauses).push_back(std::shared_ptr<Clause>(c2));
    (*clauses).push_back(std::shared_ptr<Clause>(c3));
    (*clauses).push_back(std::shared_ptr<Clause>(c4));
    (*clauses).push_back(std::shared_ptr<Clause>(c5));
    (*clauses).push_back(std::shared_ptr<Clause>(c6));
    (*clauses).push_back(std::shared_ptr<Clause>(c8));
    (*clauses2).push_back(std::shared_ptr<Clause>(new Clause(*c1)));
    (*clauses2).push_back(std::shared_ptr<Clause>(new Clause(*c2)));
    (*clauses2).push_back(std::shared_ptr<Clause>(new Clause(*c3)));
    (*clauses2).push_back(std::shared_ptr<Clause>(new Clause(*c4)));
    (*clauses2).push_back(std::shared_ptr<Clause>(new Clause(*c5)));
    (*clauses2).push_back(std::shared_ptr<Clause>(new Clause(*c6)));
    (*clauses2).push_back(std::shared_ptr<Clause>(c7));
    (*clauses2).push_back(std::shared_ptr<Clause>(new Clause(*c8)));   
}
void DpllTests::testBasicUsageWithoutWL() {
    Formula *f;
    Affectation *solution;
    std::vector<std::shared_ptr<Clause>> clauses;
    std::vector<std::shared_ptr<Clause>> clauses2 ;

    get_test_clauses(&clauses, &clauses2);

    f = new Formula(clauses, 3);
    solution = solve(f);
    CPPUNIT_ASSERT(solution->is_false(1));
    CPPUNIT_ASSERT(solution->is_true(2));
    CPPUNIT_ASSERT(solution->is_true(3));
    delete solution ;
    delete f ;
    f = new Formula(clauses2, 3);
    CPPUNIT_ASSERT_THROW(solve(f), Conflict);
    delete f ;
}
void DpllTests::testBasicUsageWithWL() {
    Formula *f;
    Affectation *solution;
    std::vector<std::shared_ptr<Clause>> clauses;
    std::vector<std::shared_ptr<Clause>> clauses2 ;

    get_test_clauses(&clauses, &clauses2);

    f = new Formula(clauses, 3);
    solution = solve_wl(f);
    CPPUNIT_ASSERT(solution->is_false(1));
    CPPUNIT_ASSERT(solution->is_true(2));
    CPPUNIT_ASSERT(solution->is_true(3));
    delete solution ;
    delete f ;
    f = new Formula(clauses2, 3);
    CPPUNIT_ASSERT_THROW(solve_wl(f), Conflict);
    delete f ;
}

CppUnit::Test* DpllTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("DpllTests");
    suite->addTest(new CppUnit::TestCaller<DpllTests>("DpllTests_testBasicUsageWithoutWL",
                &DpllTests::testBasicUsageWithoutWL));
    suite->addTest(new CppUnit::TestCaller<DpllTests>("DpllTests_testBasicUsageWithWL",
                &DpllTests::testBasicUsageWithWL));
    return suite;
}

