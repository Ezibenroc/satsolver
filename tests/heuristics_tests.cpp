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
#include "structures/extended_formula.h"
#include "solvers/dpll.h"
#include "config.h"
#include "heuristics_tests.h"

#define EF ExtendedFormula
#define SPEF std::shared_ptr<EF>

using namespace satsolver;

void HeuristicsTests::testRandom() {
    std::vector<std::shared_ptr<Clause>> g ;
    std::unordered_set <int> v = {1,2,-3} ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3,v))) ;
    v = {-1,2} ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3,v))) ;
    v = {-1,-2,-3} ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3,v))) ;
    v = {-3} ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3,v))) ;
    v = {1,2,3} ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3,v))) ;
    Formula *f = new Formula(g,3) ;
    std::vector<int> literals = f->get_unknown_literals() ;
    CPPUNIT_ASSERT(literals.size() == 4) ;
		CPPUNIT_ASSERT(literals[0]+literals[1]+literals[2]+literals[3] == 4) ;
    CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{-1,2},{1,2}}));
    int n ;
    bool a,b,c ;
    a = false ; b = false ; c = false ;
    for(int i = 0 ; i < 1000 ; i++) {
    	n = f->choose_literal_random() ;
    	CPPUNIT_ASSERT(n == 1 || n == 2 || n == -1) ;
    	if(n==1)
    		a = true ;
    	if(n==2)
    		b = true ;
    	if(n==-1)
    		c = true ;
    }
    CPPUNIT_ASSERT(a && b && c) ;
    f->bet_true(1) ;
    for(int i = 0 ; i < 1000 ; i++) {
    	CPPUNIT_ASSERT(f->choose_literal_random() == 2) ;
    }
    delete f;
}

void HeuristicsTests::testMoms() {
    std::vector<std::shared_ptr<Clause>> g ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {1,2,-3}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {1,2,3,4}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {-1,-2,-3}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {-1,2,3,4}))) ;
    Formula *f = new Formula(g,4) ;
    CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{1,2,-3},{1,2,3,4},{-1,-2,-3},{-1,2,3,4}}));
    CPPUNIT_ASSERT(f->choose_literal_moms() == -3) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, { 1,-4 }))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, { 2,-4 }))) ;
    delete f;
    f = new Formula(g,4) ;
    CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{1,2,-3},{1,2,3,4},{-1,-2,-3},{-1,2,3,4},{1,-4},{2,-4}}));
    CPPUNIT_ASSERT(f->choose_literal_moms() == -4) ;
    delete f;
}

void HeuristicsTests::testDlis() {
    std::vector<std::shared_ptr<Clause>> g ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {1,2,-3}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {1,2,3,4}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {-1,-2,-3}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, {-1,2,3,4}))) ;
    Formula *f = new Formula(g,4) ;
    CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{1,2,-3},{1,2,3,4},{-1,-2,-3},{-1,2,3,4}}));
    CPPUNIT_ASSERT(f->choose_literal_dlis() == -3 || f->choose_literal_dlis() == 2) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, { 1,-4 }))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(4, { 2,-4 }))) ;
    delete f;
    f = new Formula(g,4) ;
    CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{1,2,-3},{1,2,3,4},{-1,-2,-3},{-1,2,3,4},{1,-4},{2,-4}}));
    CPPUNIT_ASSERT(f->choose_literal_dlis() == -4 || f->choose_literal_dlis() == 2) ;
    delete f;
}


CppUnit::Test* HeuristicsTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("HeuristicsTests");
    suite->addTest(new CppUnit::TestCaller<HeuristicsTests>("StructuresTests_testRandom",
                &HeuristicsTests::testRandom));
    suite->addTest(new CppUnit::TestCaller<HeuristicsTests>("StructuresTests_testMoms",
                &HeuristicsTests::testMoms));
    suite->addTest(new CppUnit::TestCaller<HeuristicsTests>("StructuresTests_testDlis",
                &HeuristicsTests::testDlis));
    return suite;
}
