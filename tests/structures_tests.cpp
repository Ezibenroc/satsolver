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
    satsolver::Clause *clause = new Clause(5, v);
    CPPUNIT_ASSERT(!clause->contains_literal(-5));
    CPPUNIT_ASSERT(!clause->contains_literal(-4));
    CPPUNIT_ASSERT(!clause->contains_literal(-3));
    CPPUNIT_ASSERT(clause->contains_literal(-2));
    CPPUNIT_ASSERT(!clause->contains_literal(-1));
    CPPUNIT_ASSERT(!clause->contains_literal(1));
    CPPUNIT_ASSERT(!clause->contains_literal(2));
    CPPUNIT_ASSERT(clause->contains_literal(3));
    CPPUNIT_ASSERT(!clause->contains_literal(4));
    CPPUNIT_ASSERT(!clause->contains_literal(5));
    satsolver::Clause *c2 = new Clause(*clause) ;
    CPPUNIT_ASSERT(c2->contains_clause(*clause)) ;
    CPPUNIT_ASSERT(clause->contains_clause(*c2)) ;
    clause->remove(3) ; 
    CPPUNIT_ASSERT(c2->contains_clause(*clause)) ;
    CPPUNIT_ASSERT(!clause->contains_clause(*c2)) ;
    clause->remove(2) ;
    CPPUNIT_ASSERT(!c2->contains_literal(-5));
    CPPUNIT_ASSERT(!c2->contains_literal(-4));
    CPPUNIT_ASSERT(!c2->contains_literal(-3));
    CPPUNIT_ASSERT(c2->contains_literal(-2));
    CPPUNIT_ASSERT(!c2->contains_literal(-1));
    CPPUNIT_ASSERT(!c2->contains_literal(1));
    CPPUNIT_ASSERT(!c2->contains_literal(2));
    CPPUNIT_ASSERT(c2->contains_literal(3));
    CPPUNIT_ASSERT(!c2->contains_literal(4));
    CPPUNIT_ASSERT(!c2->contains_literal(5));
    delete clause ;
    delete c2 ;
}

void StructuresTests::testBasicUsage() {
    std::vector <int> v = {1,2,-3,4,} ;
    Clause *c = new Clause(4,v) ;
    Affectation *aff = new Affectation(4) ;
    c->set_affectation(aff) ;
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
    c->remove(-1) ;c->remove(1) ; c->remove(2) ; c->remove(-2) ; c->remove(-3) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({3,4}));
    c->remove(3) ; c->remove(4) ;
    CPPUNIT_ASSERT(c->to_set() == std::set<int>({}));
    CPPUNIT_ASSERT(a->to_set() == std::set<int>({1,-2,2,-3,3,4}));
    delete a;
    delete c;
}



void StructuresTests::testAffectationCreationUsage(){
    satsolver::Affectation aff(3) ;
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 3) ;
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
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 2) ;
    aff.set_false(3);
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 1) ;
    CPPUNIT_ASSERT(aff.is_unknown(1));
    CPPUNIT_ASSERT(!aff.is_unknown(2));
    CPPUNIT_ASSERT(!aff.is_unknown(3));
    CPPUNIT_ASSERT(!aff.is_true(1));
    CPPUNIT_ASSERT(aff.is_true(2));
    CPPUNIT_ASSERT(!aff.is_true(3));
    CPPUNIT_ASSERT(!aff.is_false(1));
    CPPUNIT_ASSERT(!aff.is_false(2));
    CPPUNIT_ASSERT(aff.is_false(3));
    aff.set_false(3);
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 1) ;
    aff.set_true(3);
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 1) ;
    aff.set_unknown(3);
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 2) ;
}

void StructuresTests::testSetTrueClause() {
    std::vector <int> v = {1,2,-3,4,} ;
    Clause *c = new Clause(4,v) ;	
    satsolver::Affectation *aff = new satsolver::Affectation(4) ;
    c->set_affectation(aff) ;
    c->init_WL() ;
    CPPUNIT_ASSERT(c->fst_WL()!=c->snd_WL() && c->contains_literal(c->fst_WL()) && c->contains_literal(c->snd_WL())) ;
    CPPUNIT_ASSERT(c->set_false(1) == 0) ;
    aff->set_false(1) ;
    CPPUNIT_ASSERT(c->set_false(2) == 0) ;
    aff->set_false(2) ;
    CPPUNIT_ASSERT((c->fst_WL()==-3 && c->snd_WL()==4)||(c->fst_WL()==4 && c->snd_WL()==-3));
    CPPUNIT_ASSERT(c->set_false(4) == -3) ;
    CPPUNIT_ASSERT((c->fst_WL()==-3 && c->snd_WL()==4)||(c->fst_WL()==4 && c->snd_WL()==-3));
    CPPUNIT_ASSERT(c->set_false(-3) == 4) ;
    aff->set_unknown(1) ; aff->set_unknown(2) ;
    CPPUNIT_ASSERT(c->set_true(1) == 0) ;
    aff->set_true(1) ;
    CPPUNIT_ASSERT(c->set_true(2) == 0) ;
    aff->set_true(2) ;
    CPPUNIT_ASSERT(c->fst_WL()==1 || c->snd_WL()==1 || c->fst_WL()==2 || c->snd_WL()==2);
    delete aff ;
    delete c ;
}

void StructuresTests::testFormula() {
    std::vector<std::shared_ptr<Clause>> g ;
    std::vector <int> v = {1,2,-3} ;
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
   	CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{-1,2},{1,2}}));
   	std::stack<std::pair<int,bool>> st = f->get_mem() ;
    CPPUNIT_ASSERT(st.size() == 1 && (st.top() == std::pair<int,bool>(-3,true))) ;
    CPPUNIT_ASSERT(f->get_aff()->is_true(-3)) ;    
    CPPUNIT_ASSERT(f->get_aff()->get_nb_unknown() == 2) ;
    
		Formula *f2 = new Formula(f) ;
   	CPPUNIT_ASSERT(f2->to_set() == std::set<std::set<int>>({{-1,2},{1,2}}));
   	CPPUNIT_ASSERT(f2->isolated_literal() == 2) ;
   	CPPUNIT_ASSERT(f2->bet_true(-1)) ;
    CPPUNIT_ASSERT(f2->get_aff()->is_true(-1) && f2->get_aff()->is_true(2) && f2->get_aff()->is_true(-3)) ;
    CPPUNIT_ASSERT(f2->get_aff()->get_nb_unknown() == 0) ;
    st = f2->get_mem() ;
    CPPUNIT_ASSERT(st.size() == 3) ;
    CPPUNIT_ASSERT((st.top() == std::pair<int,bool>(2,true))) ;
    st.pop() ;
    CPPUNIT_ASSERT((st.top() == std::pair<int,bool>(-1,false))) ;
		st.pop() ;
    CPPUNIT_ASSERT((st.top() == std::pair<int,bool>(-3,true))) ;
   	CPPUNIT_ASSERT(f2->to_set() == std::set<std::set<int>>({}));
		delete f2 ;
		
		CPPUNIT_ASSERT(!f->bet_false(2)) ;
    CPPUNIT_ASSERT(f->get_aff()->is_unknown(1) && f->get_aff()->is_false(2) && f->get_aff()->is_true(-3)) ;
    CPPUNIT_ASSERT(f->get_aff()->get_nb_unknown() == 1) ;
    st = f->get_mem() ;
    CPPUNIT_ASSERT(st.size() == 2) ;
    CPPUNIT_ASSERT((st.top() == std::pair<int,bool>(-2,false))) ;
		st.pop() ;
    CPPUNIT_ASSERT((st.top() == std::pair<int,bool>(-3,true))) ;
   	CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{-1},{1}}));
   	CPPUNIT_ASSERT(f->isolated_literal() == 0) ;
   	
   	CPPUNIT_ASSERT(f->back() == -2) ;
   	CPPUNIT_ASSERT(f->to_set() == std::set<std::set<int>>({{-1,2},{1,2}}));   
    CPPUNIT_ASSERT(f->get_aff()->get_nb_unknown() == 2) ;
   	CPPUNIT_ASSERT(f->back() == 0) ;
    delete f ;

}

CppUnit::Test* StructuresTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("StructuresTests");
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testClauseCreation",
                &StructuresTests::testClauseCreation));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testBasicUsage",
                &StructuresTests::testBasicUsage));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testAffectationCreationUsage",
                &StructuresTests::testAffectationCreationUsage));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testSetTrueClause",
                &StructuresTests::testSetTrueClause));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testFormula",
                &StructuresTests::testFormula));
    return suite;
}
