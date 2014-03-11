#include <sstream>
#include <iostream>
#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "parsers/graph.h"

#include "graphparser_tests.h"

void GraphParserTests::testBasic() {
    std::istringstream stream("p edge 5 3\ne 3 0\ne 1 2\ne 4 1\ne 4 3\n");
    std::istringstream &stream2 = stream;
    graphsolver::Graph *graph;
    graphsolver::GraphParser *parser;
    parser = new graphsolver::GraphParser(stream2);
    CPPUNIT_ASSERT_NO_THROW(parser->parse());
    graph = parser->get_graph();
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(0) == std::set<int>({}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(1) == std::set<int>({}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(2) == std::set<int>({1}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(3) == std::set<int>({0}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(4) == std::set<int>({1, 3}));
    delete parser;
    delete graph;
}
void GraphParserTests::testExtraWhitespaces() {
    std::istringstream stream("p   edge 5   3\ne    3  0\n\ne   1  2   \ne 4 1\n\n\ne 4 3");
    std::istringstream &stream2 = stream;
    graphsolver::Graph *graph;
    graphsolver::GraphParser *parser;
    parser = new graphsolver::GraphParser(stream2);
    CPPUNIT_ASSERT_NO_THROW(parser->parse());
    graph = parser->get_graph();
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(0) == std::set<int>({}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(1) == std::set<int>({}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(2) == std::set<int>({1}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(3) == std::set<int>({0}));
    CPPUNIT_ASSERT(*graph->get_lower_adjacent_nodes(4) == std::set<int>({1, 3}));
    delete parser;
    delete graph;
}


CppUnit::Test* GraphParserTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("GraphParserTests");
    suite->addTest(new CppUnit::TestCaller<GraphParserTests>("GraphParserTest_testBasic",
                &GraphParserTests::testBasic));
    suite->addTest(new CppUnit::TestCaller<GraphParserTests>("GraphParserTest_testExtraWhitespaces",
                &GraphParserTests::testExtraWhitespaces));
    return suite;
}

