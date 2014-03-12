#include <sstream>
#include <iostream>
#include <cppunit/Test.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>

#include "structures/graph.h"
#include "structures/extended_formula.h"
#include "solvers/dpll.h"
#include "solvers/graphcolor.cpp"

#include "graphsolver_tests.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>

// Test generation of formula representing edges
void GraphSolverTests::testEdgeFormula() {
    SPEF formula = get_color_disjonction_of_edge(2, 5, 3);
    // The following formula may vary; change it if needed if the implementation
    // of get_color_disjonction_of_edge() or EF::to_string() changes in the
    // future (but check it manually before changing it).
    CPPUNIT_ASSERT(formula->to_string() == "(((“2 0”⊕“5 0”)+(“2 1”⊕“5 1”))+(“2 2”⊕“5 2”))");
}

// Test generation of the “maximum color” formula
void GraphSolverTests::testMaxFormula() {
    SPEF formula;
    formula = get_color_limitation_formula_for_node(2, 1, 3);
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 2”)•((¬“2 1”)•(¬“2 0”)))");
    formula = get_color_limitation_formula_for_node(2, 2, 3);
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 2”)•(¬“2 1”))");
    formula = get_color_limitation_formula_for_node(2, 3, 3);
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 2”)•((¬“2 1”)+(“2 1”•(¬“2 0”))))");
    formula = get_color_limitation_formula_for_node(2, 4, 3);
    formula = formula->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "(¬“2 2”)");
}

// Test the solver
void GraphSolverTests::testSolve() {
    graphsolver::Graph graph(5, 1);
    graph.add_edge(3, 0);
    graph.add_edge(1, 2);
    graph.add_edge(4, 1);
    graph.add_edge(4, 3);
    /*     3
     *     |
     * 0---4---1---2
     */
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(1, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(2, &graph));
}


CppUnit::Test* GraphSolverTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("GraphSolverTests");
    suite->addTest(new CppUnit::TestCaller<GraphSolverTests>("GraphSolverTest_testEdgeFormula",
                &GraphSolverTests::testEdgeFormula));
    suite->addTest(new CppUnit::TestCaller<GraphSolverTests>("GraphSolverTest_testMaxFormula",
                &GraphSolverTests::testMaxFormula));
    suite->addTest(new CppUnit::TestCaller<GraphSolverTests>("GraphSolverTest_testSolve",
                &GraphSolverTests::testSolve));
    return suite;
}


