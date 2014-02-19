#include <sstream>
#include <iostream>
#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "parser.cpp"

#include "parser_tests.h"

void ParserTests::testBasic() {
    std::istringstream stream("p cnf 5 3\n3 0\n1 2 4 0\n5 1 0\n");
    std::istringstream &stream2 = stream;
    std::vector<satsolver::Clause*> clauses;
    std::set<std::set<int>> clauses_set;
    satsolver::Formula *formula;
    satsolver::Parser *parser;
    CPPUNIT_ASSERT_NO_THROW(parser = new satsolver::Parser(stream2));
    clauses = parser->get_clauses();
    CPPUNIT_ASSERT(clauses[0]->to_set() == std::set<int>({3}));
    CPPUNIT_ASSERT(clauses[1]->to_set() == std::set<int>({1, 2, 4}));
    CPPUNIT_ASSERT(clauses[2]->to_set() == std::set<int>({1, 5}));
    CPPUNIT_ASSERT(clauses[0]->to_set() == std::set<int>({3}));
    formula = parser->get_formula();
    clauses_set.insert(clauses[0]->to_set());
    clauses_set.insert(clauses[1]->to_set());
    clauses_set.insert(clauses[2]->to_set());
    CPPUNIT_ASSERT(formula->to_set() == clauses_set);
    delete parser;
}
void ParserTests::testExtraWhitespaces() {
    std::istringstream stream("c 0 foo bar\np cnf 5 3\n3 0\n\n\n1 2      4 0\nc oof\n5 1 0\n");
    std::istringstream &stream2 = stream;
    std::vector<satsolver::Clause*> clauses;
    satsolver::Parser *parser;
    CPPUNIT_ASSERT_NO_THROW(parser = new satsolver::Parser(stream2));
    clauses = parser->get_clauses();
    CPPUNIT_ASSERT(clauses[0]->to_set() == std::set<int>({3}));
    CPPUNIT_ASSERT(clauses[1]->to_set() == std::set<int>({1, 2, 4}));
    CPPUNIT_ASSERT(clauses[2]->to_set() == std::set<int>({1, 5}));
    delete parser;
}


CppUnit::Test* ParserTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("ParserTests");
    suite->addTest(new CppUnit::TestCaller<ParserTests>("testBasic",
                &ParserTests::testBasic));
    suite->addTest(new CppUnit::TestCaller<ParserTests>("testExtraWhitespaces",
                &ParserTests::testExtraWhitespaces));
    return suite;
}
