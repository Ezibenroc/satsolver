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
    SPEF formula = get_color_disjonction_of_edge(2, 5, 3)->simplify();
    // The following formula may vary; change it if needed if the implementation
    // of get_color_disjonction_of_edge() or EF::to_string() changes in the
    // future (but check it manually before changing it).
    CPPUNIT_ASSERT(formula->to_string() == "(((“2 0”⊕“5 0”)+(“2 1”⊕“5 1”))+(“2 2”⊕“5 2”))");
}

// Test generation of the “maximum color” formula
void GraphSolverTests::testMaxFormula() {
    SPEF formula;
    formula = get_color_limitation_formula_for_node(2, 1, 2)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 1”)•(¬“2 0”))");
    formula = get_color_limitation_formula_for_node(2, 2, 2)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "(¬“2 1”)");
    formula = get_color_limitation_formula_for_node(2, 3, 2)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 1”)+(“2 1”•(¬“2 0”)))");
    formula = get_color_limitation_formula_for_node(2, 1, 3)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 2”)•((¬“2 1”)•(¬“2 0”)))");
    formula = get_color_limitation_formula_for_node(2, 2, 3)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 2”)•(¬“2 1”))");
    formula = get_color_limitation_formula_for_node(2, 3, 3)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "((¬“2 2”)•((¬“2 1”)+(“2 1”•(¬“2 0”))))");
    formula = get_color_limitation_formula_for_node(2, 4, 3)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "(¬“2 2”)");

    formula = get_color_limitation_formula(2, 2, 2)->simplify();
    CPPUNIT_ASSERT(formula->to_string() == "((¬“0 1”)•(¬“1 1”))");
}

// Test the solver
void GraphSolverTests::testSolve() {
    graphsolver::ColorAffectation *solution;
    graphsolver::Graph graph(5, 1);
    graph.add_edge(4, 0);
    graph.add_edge(1, 2);
    graph.add_edge(4, 1);
    graph.add_edge(4, 3);
    /*     3
     *     |
     * 0---4---1---2
     */
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(2, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(3, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(4, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(5, &graph));

    graph.add_edge(1, 3);
    /*       3
     *      / \
     * 0---4---1---2
     */
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(2, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(3, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(4, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(5, &graph));

    graph.add_edge(2, 3);
    /*         3
     *       / | \
     * 0---4---1---2
     */
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(2, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(3, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(4, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(5, &graph));

    graph.add_edge(4, 2);
    /*         3
     *       / | \
     * 0---4---1---2
     *      \_____/
     */
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(2, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(3, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(4, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(5, &graph));

    graph.add_edge(0, 1);
    graph.add_edge(0, 2);
    graph.add_edge(0, 3);
    // Complete graph
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(2, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(3, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(4, &graph), satsolver::Conflict);
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(5, &graph));
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(6, &graph));
}

void GraphSolverTests::testSolveExtremeCases() {
    graphsolver::Graph graph(5, 1);
    CPPUNIT_ASSERT_NO_THROW(graphsolver::solve_colors(1, &graph));
    graph.add_edge(3, 0);
    graph.add_edge(1, 2);
    graph.add_edge(4, 1);
    graph.add_edge(4, 3);
    /*     3
     *     |
     * 0---4---1---2
     */
    CPPUNIT_ASSERT_THROW(graphsolver::solve_colors(1, &graph), satsolver::Conflict);

}


CppUnit::Test* GraphSolverTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("GraphSolverTests");
    suite->addTest(new CppUnit::TestCaller<GraphSolverTests>("GraphSolverTest_testEdgeFormula",
                &GraphSolverTests::testEdgeFormula));
    suite->addTest(new CppUnit::TestCaller<GraphSolverTests>("GraphSolverTest_testMaxFormula",
                &GraphSolverTests::testMaxFormula));
    suite->addTest(new CppUnit::TestCaller<GraphSolverTests>("GraphSolverTest_testSolve",
                &GraphSolverTests::testSolve));
    suite->addTest(new CppUnit::TestCaller<GraphSolverTests>("GraphSolverTest_testSolveExtremeCases",
                &GraphSolverTests::testSolveExtremeCases));
    return suite;
}


