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
#include "structures_tests.h"

using namespace satsolver;

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
  CPPUNIT_ASSERT(!c->is_tautology()) ;
	c->add(-2) ;
  CPPUNIT_ASSERT(c->is_tautology()) ;
  CPPUNIT_ASSERT(c->to_set() == std::set<int>({1,-2,2,-3,4}));
	c->add(3) ;
  CPPUNIT_ASSERT(c->is_tautology()) ;
	Clause *a = new Clause(*c) ;
  CPPUNIT_ASSERT(a->to_set() == std::set<int>({1,-2,2,-3,3,4}));
  CPPUNIT_ASSERT(a->is_tautology()) ;
/*	c->remove(-1) ;c->remove(1) ; c->remove(2) ; c->remove(-2) ; c->remove(-3) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({3,4}));
	c->remove(3) ;	c->remove(4) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({}));
    CPPUNIT_ASSERT(a->to_set() == std::set<int>({1,-2,2,-3,3,4}));*/
    delete a;
    delete c;
}

void StructuresTests::testAffectationCreationUsage(){
	satsolver::Affectation aff(3) ;
  CPPUNIT_ASSERT(aff.is_unknown(1));
  CPPUNIT_ASSERT(aff.is_unknown(2));
  CPPUNIT_ASSERT(aff.is_unknown(3));
  CPPUNIT_ASSERT(!aff.is_true(1));
  CPPUNIT_ASSERT(!aff.is_true(2));
  CPPUNIT_ASSERT(!aff.is_true(3));
  CPPUNIT_ASSERT(!aff.is_false(1));
  CPPUNIT_ASSERT(!aff.is_false(2));
  CPPUNIT_ASSERT(!aff.is_false(3));
  aff.set_true(2) ;
  aff.set_false(3);
  CPPUNIT_ASSERT(aff.is_unknown(1));
  CPPUNIT_ASSERT(!aff.is_unknown(2));
  CPPUNIT_ASSERT(!aff.is_unknown(3));
  CPPUNIT_ASSERT(!aff.is_true(1));
  CPPUNIT_ASSERT(aff.is_true(2));
  CPPUNIT_ASSERT(!aff.is_true(3));
  CPPUNIT_ASSERT(!aff.is_false(1));
  CPPUNIT_ASSERT(!aff.is_false(2));
  CPPUNIT_ASSERT(aff.is_false(3));
}
void StructuresTests::testAffClauses() {
	satsolver::Affectation aff(3) ;
	std::vector <int> v = {1,2,-3} ;
	Clause *c = new Clause(3,v) ;
	CPPUNIT_ASSERT(!c->is_true()) ;
	CPPUNIT_ASSERT(!c->is_false()) ;
	CPPUNIT_ASSERT(!c->monome(aff)) ;
	aff.set_true(2) ;
	c->set_true(2) ;
	CPPUNIT_ASSERT(c->is_true()) ;
	CPPUNIT_ASSERT(!c->is_false()) ;
	CPPUNIT_ASSERT(!c->monome(aff)) ;	
	aff.set_false(2) ;
	c->unset_true(2) ;
	c->set_false(2) ;
	aff.set_true(-3) ;
	c->set_true(-3) ;
	CPPUNIT_ASSERT(c->is_true()) ;
	CPPUNIT_ASSERT(!c->is_false()) ;
	CPPUNIT_ASSERT(!c->monome(aff)) ;
	aff.set_false(-3) ;
	c->unset_true(-3) ;
	c->set_false(-3) ;
	CPPUNIT_ASSERT(!c->is_true()) ;
	CPPUNIT_ASSERT(!c->is_false()) ;
	CPPUNIT_ASSERT(c->monome(aff)==1) ;
	aff.set_unknown(-3) ;
	c->unset_false(-3) ;
	aff.set_false(1) ;
	c->set_false(1) ;
	CPPUNIT_ASSERT(!c->is_true()) ;
	CPPUNIT_ASSERT(!c->is_false()) ;
	CPPUNIT_ASSERT(c->monome(aff)==-3) ;
	aff.set_false(-3) ;
	c->set_false(-3) ;
	CPPUNIT_ASSERT(!c->is_true()) ;
	CPPUNIT_ASSERT(c->is_false()) ;
	CPPUNIT_ASSERT(!c->monome(aff)) ;
	delete c ;
}

void StructuresTests::testUnitPropagation() {
	std::vector <int> v = {1,2,-3} ;
	Clause *c = new Clause(3,v) ;
	std::vector <int> w = {-1,2} ;
	Clause *d = new Clause(3,w) ;
	std::vector <int> x = {-1,-2,-3} ;
	Clause *e = new Clause(3,x) ;
	std::vector <int> y = {1} ;
	Clause *b = new Clause(3,y) ;		
	std::vector<Clause*> g ;
	g.push_back(c) ;
	g.push_back(d) ;
	g.push_back(e) ;
	g.push_back(b) ;
	Formula *f = new Formula(g,3) ;
  CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{1,2,-3},{-1,2},{-1,-2,-3},{1}}));
	CPPUNIT_ASSERT(f->unit_propagation()) ;
 	CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{2},{-2,-3}}));
	CPPUNIT_ASSERT(f->unit_propagation()) ;
 	CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{-3}}));
	CPPUNIT_ASSERT(f->unit_propagation()) ;
 	CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({}));
	CPPUNIT_ASSERT(!f->unit_propagation()) ;
    delete f;
}

CppUnit::Test* StructuresTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("StructuresTests");
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testClauseCreation",
                &StructuresTests::testClauseCreation));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testBasicUsage",
                &StructuresTests::testBasicUsage));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testAffectationCreationUsage",
                &StructuresTests::testAffectationCreationUsage));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testAffClauses",
                &StructuresTests::testAffClauses));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("testUnitPropagation",
                &StructuresTests::testUnitPropagation));
    return suite;
}
