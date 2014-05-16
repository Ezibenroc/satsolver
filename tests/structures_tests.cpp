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

#include "solvers/void_assistant.h"
#include "structures/clause.h"
#include "structures/formula.h"
#include "structures/affectation.h"
#include "structures/extended_formula.h"
#include "structures/adj_graph.h"
#include "solvers/dpll.h"
#include "structures_tests.h"
#include "config.h"

#define EF ExtendedFormula
#define SPEF std::shared_ptr<EF>

using namespace satsolver;

void StructuresTests::testClauseCreation() {
    std::unordered_set<int> v({3, -2});
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
    Clause *c = new Clause(4,{1,2,-3,4,}) ;
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
    delete aff;
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
    aff.set_unknown(3) ;
    aff.set_false(3);
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 1) ;
    aff.set_unknown(3) ;
    aff.set_true(3);
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 1) ;
    aff.set_unknown(3);
    CPPUNIT_ASSERT(aff.get_nb_unknown() == 2) ;
}

void StructuresTests::testSetTrueClause() {
    WITH_WL = true;
    Clause *c = new Clause(4, {1,2,-3,4,}) ;
    satsolver::Affectation *aff = new satsolver::Affectation(4) ;
    c->set_affectation(aff) ;
    c->init_WL() ;
    CPPUNIT_ASSERT(c->fst_WL()!=c->snd_WL() && c->contains_literal(c->fst_WL()) && c->contains_literal(c->snd_WL())) ;
    CPPUNIT_ASSERT(c->set_false(1) == 0) ;
    aff->set_false(1) ;
    CPPUNIT_ASSERT(c->set_false(2) == 0) ;
    aff->set_false(2) ;
    CPPUNIT_ASSERT(c->is_WL(-3) && c->is_WL(4));
    CPPUNIT_ASSERT((c->fst_WL()==-3 && c->snd_WL()==4)||(c->fst_WL()==4 && c->snd_WL()==-3));
    CPPUNIT_ASSERT(c->set_false(4) == -3) ;
    CPPUNIT_ASSERT((c->fst_WL()==-3 && c->snd_WL()==4)||(c->fst_WL()==4 && c->snd_WL()==-3));
    CPPUNIT_ASSERT(c->set_false(-3) == 4) ;
    aff->set_unknown(1) ; aff->set_unknown(2) ;
    CPPUNIT_ASSERT(c->set_true(1) == 0) ;
    aff->set_true(1) ;
    CPPUNIT_ASSERT(c->set_true(2) == 0) ;
    aff->set_true(2) ;
    CPPUNIT_ASSERT(c->is_WL(1) && c->is_WL(2));
    delete aff ;
    delete c ;

    c = new Clause(3, {1,2,3}) ;
    aff = new satsolver::Affectation(3) ;
    c->set_affectation(aff) ;
    c->init_WL() ;
    c->set_false(1) ;
    CPPUNIT_ASSERT(!c->is_WL(1)) ;
    delete c ;
    delete aff ;
    WITH_WL = false;
}

void StructuresTests::testFormula() {
    int clause_id;
    theorysolver::VoidAssistant assistant;

    std::vector<std::shared_ptr<Clause>> g ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3, {1,2,-3}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3, {-1,2}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3, {-1,-2,-3}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3, {-3}))) ;
    g.push_back(std::shared_ptr<Clause>(new Clause(3, {1,2,3}))) ;
    Formula f(g,3) ;
    CPPUNIT_ASSERT(f.to_set() == std::set<std::set<int>>({{-1,2},{1,2}}));
    std::vector<std::pair<int,bool>> st = f.get_mem() ;
    CPPUNIT_ASSERT(st.size() == 1 && (st.back() == std::pair<int,bool>(-3,true))) ;
    CPPUNIT_ASSERT(f.get_aff()->is_true(-3)) ;
    CPPUNIT_ASSERT(f.get_aff()->get_nb_unknown() == 2) ;

    Formula f2(f) ;
    CPPUNIT_ASSERT(f2.to_set() == std::set<std::set<int>>({{-1,2},{1,2}}));
    CPPUNIT_ASSERT(f2.isolated_literal(NULL) == 2) ;
    CPPUNIT_ASSERT(f2.bet_true(-1,NULL,NULL,NULL, &assistant,NULL)) ;
    CPPUNIT_ASSERT(!f2.contains_false_clause(NULL));

    CPPUNIT_ASSERT(f2.monome(NULL) == 2) ;

    f2.bet_false(2,NULL,NULL,NULL, &assistant,NULL);
    CPPUNIT_ASSERT(f2.contains_false_clause(&clause_id));
    CPPUNIT_ASSERT(f2.to_clauses_vector()[clause_id]->to_string() == "{1,2}");
/*    CPPUNIT_ASSERT(f2->get_aff()->is_true(-1) && f2->get_aff()->is_true(2) && f2->get_aff()->is_true(-3)) ;
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
    CPPUNIT_ASSERT(f->back() == 0) ;*/

}

void StructuresTests::testAdjGraph() {
    theorysolver::AdjGraph g;
    std::pair<std::list<theorysolver::path_item>, int> r;
    std::list<theorysolver::path_item>::iterator it;
    g.add_edge(4, 6, 10, 1);
    g.add_edge(6, 2, 11, 4);
    g.add_edge(4, 3, 12, -1);
    g.add_edge(3, 1, 13, 2);
    g.add_edge(1, 2, 14, 1);
    r = g.find_lowest_path(4, 2);
    it = r.first.begin();
    CPPUNIT_ASSERT(it != r.first.end());
    CPPUNIT_ASSERT(it->node == 3);
    CPPUNIT_ASSERT(it->tag == 12);
    it++;
    CPPUNIT_ASSERT(it != r.first.end());
    CPPUNIT_ASSERT(it->node == 1);
    CPPUNIT_ASSERT(it->tag == 13);
    it++;
    CPPUNIT_ASSERT(it != r.first.end());
    CPPUNIT_ASSERT(it->node == 2);
    CPPUNIT_ASSERT(it->tag == 14);
    it++;
    CPPUNIT_ASSERT(it == r.first.end());
    CPPUNIT_ASSERT(r.second == 2);

    g.delete_edge(3, 1, 13);
    r = g.find_lowest_path(4, 2);
    it = r.first.begin();
    CPPUNIT_ASSERT(it != r.first.end());
    CPPUNIT_ASSERT(it->node == 6);
    CPPUNIT_ASSERT(it->tag == 10);
    it++;
    CPPUNIT_ASSERT(it != r.first.end());
    CPPUNIT_ASSERT(it->node == 2);
    CPPUNIT_ASSERT(it->tag == 11);
    it++;
    CPPUNIT_ASSERT(it == r.first.end());
    CPPUNIT_ASSERT(r.second == 5);

    r = g.find_lowest_path(2, 19);
    r = g.find_lowest_path(10, 15);

}

void StructuresTests::testExtendedFormula() {
    std::shared_ptr<std::map<std::string, int>> variable_to_literal;
    std::shared_ptr<Formula> formula;
    SPEF extended_formula =
            SPEF(new EF(EF::XOR,
                SPEF(new EF(EF::AND,
                    SPEF(new EF(EF::LITERAL, "foo")),
                    SPEF(new EF(EF::LITERAL, "bar"))
                )),
                SPEF(new EF(EF::AND,
                    SPEF(new EF(EF::LITERAL, "foo")),
                    SPEF(new EF(EF::NOT,
                        SPEF(new EF(EF::LITERAL, "bar"))
                    ))
                ))
            ));
    formula = extended_formula->reduce_to_formula(&variable_to_literal);
    CPPUNIT_ASSERT(variable_to_literal);
    CPPUNIT_ASSERT(variable_to_literal->size() >= 2);
    CPPUNIT_ASSERT(variable_to_literal->find("foo") != variable_to_literal->end());
    Affectation *sat_solution = satsolver::solve(formula);
    CPPUNIT_ASSERT(sat_solution->is_true(variable_to_literal->at("foo")));

    extended_formula =
            SPEF(new EF(EF::AND,
                SPEF(new EF(EF::XOR,
                    SPEF(new EF(EF::LITERAL, "foo")),
                    SPEF(new EF(EF::LITERAL, "bar"))
                )),
                SPEF(new EF(EF::AND,
                    SPEF(new EF(EF::LITERAL, "foo")),
                    SPEF(new EF(EF::LITERAL, "bar"))
                ))
            ));
    variable_to_literal->clear();
    CPPUNIT_ASSERT_THROW((
        formula = extended_formula->reduce_to_formula(&variable_to_literal),
        satsolver::solve(formula)), satsolver::Conflict);



    extended_formula =
            SPEF(new EF(EF::AND,
                SPEF(new EF(EF::IMPLIES,
                    SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "bar")))),
                    SPEF(new EF(EF::OR,
                        SPEF(new EF(EF::LITERAL, "foo")),
                        SPEF(new EF(EF::LITERAL, "bar"))
                    ))
                )),
                SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "bar"))))
            ));
    variable_to_literal->clear();
    formula = extended_formula->reduce_to_formula(&variable_to_literal);
    sat_solution = satsolver::solve(formula);
    CPPUNIT_ASSERT(sat_solution->is_true(variable_to_literal->at("foo")));

    extended_formula =
            SPEF(new EF(EF::AND,
                SPEF(new EF(EF::IMPLIES,
                    SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "bar")))),
                    SPEF(new EF(EF::AND,
                        SPEF(new EF(EF::LITERAL, "foo")),
                        SPEF(new EF(EF::LITERAL, "bar"))
                    ))
                )),
                SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "foo"))))
            ));
    variable_to_literal->clear();
    formula = extended_formula->reduce_to_formula(&variable_to_literal);
    sat_solution = satsolver::solve(formula);
    CPPUNIT_ASSERT(sat_solution->is_false(variable_to_literal->at("foo")));
    CPPUNIT_ASSERT(sat_solution->is_true(variable_to_literal->at("bar")));

    extended_formula =
            SPEF(new EF(EF::AND,
                SPEF(new EF(EF::AND,
                    SPEF(new EF(EF::IMPLIES,
                        SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "bar")))),
                        SPEF(new EF(EF::AND,
                            SPEF(new EF(EF::LITERAL, "foo")),
                            SPEF(new EF(EF::LITERAL, "bar"))
                        ))
                    )),
                    SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "foo"))))
                )),
                SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "bar"))))
            ));
    variable_to_literal->clear();
    CPPUNIT_ASSERT_THROW((
        formula = extended_formula->reduce_to_formula(&variable_to_literal),
        satsolver::solve(formula)), satsolver::Conflict);
}

void StructuresTests::testExtendedFormulaSimplification() {
    SPEF formula;
    formula = SPEF(new EF(EF::NOT, SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "foo"))))));
    CPPUNIT_ASSERT(*formula->simplify() == EF(EF::LITERAL, "foo"));
    formula = SPEF(new EF(EF::AND, SPEF(new EF(EF::LITERAL, "foo")), SPEF(new EF(EF::NOT, SPEF(new EF(EF::LITERAL, "foo"))))));
    CPPUNIT_ASSERT(*formula->simplify() == EF(EF::FALSE));
}

void StructuresTests::testExtendedFormulaComparison() {
    CPPUNIT_ASSERT(EF(EF::LITERAL, "foo") == EF(EF::LITERAL, "foo"));
    CPPUNIT_ASSERT(EF(EF::LITERAL, "foo") != EF(EF::LITERAL, "bar"));

    CPPUNIT_ASSERT(EF(EF::NOT, SPEF(new EF(EF::LITERAL, "foo"))) == EF(EF::NOT, SPEF(new EF(EF::LITERAL, "foo"))));
    CPPUNIT_ASSERT(EF(EF::NOT, SPEF(new EF(EF::LITERAL, "foo"))) != EF(EF::NOT, SPEF(new EF(EF::LITERAL, "bar"))));
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
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testAdjGraph",
                &StructuresTests::testAdjGraph));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testExtendedFormula",
                &StructuresTests::testExtendedFormula));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testExtendedFormulaComparison",
                &StructuresTests::testExtendedFormulaComparison));
    suite->addTest(new CppUnit::TestCaller<StructuresTests>("StructuresTests_testExtendedFormulaSimplification",
                &StructuresTests::testExtendedFormulaSimplification));
    return suite;
}
