cmake_minimum_required (VERSION 2.6)
project(satsolver)

add_definitions(-std=c++0x)
add_definitions(-g -Wall -Wextra -pedantic  -Wcast-qual -Wcast-align -Wconversion -Wdouble-promotion -Wfloat-equal -Wpointer-arith -fmax-errors=1)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

include_directories ("${PROJECT_SOURCE_DIR}")

add_library(Affectation src/structures/affectation.cpp)
add_library(Clause src/structures/clause.cpp)
add_library(Formula src/structures/formula.cpp)
target_link_libraries(Formula Clause Affectation)
add_library(ExtendedFormula src/structures/extended_formula.cpp)
add_library(Graph src/structures/graph.cpp)

add_library(CommandLineParser src/parsers/cli.cpp)

add_library(AbstractParser src/parsers/abstract.cpp)
add_library(SatParser src/parsers/sat.cpp)
target_link_libraries(SatParser AbstractParser Clause Formula)

add_library(GraphParser src/parsers/graph.cpp)
target_link_libraries(GraphParser AbstractParser Graph)

add_library(DPLL src/solvers/dpll.cpp)
target_link_libraries(DPLL Affectation Clause Formula)

add_library(GraphColorSolver src/solvers/graphcolor.cpp)
target_link_libraries(GraphColorSolver Graph ExtendedFormula Formula)


add_executable(resol src/entry_points/resol.cpp)
target_link_libraries(resol Affectation Formula SatParser CommandLineParser DPLL)

add_executable(colorie src/entry_points/colorie.cpp)
target_link_libraries(colorie GraphParser CommandLineParser GraphColorSolver DPLL)

find_package(CppUnit)
if (CPPUNIT_FOUND)
    add_library(StructuresTests tests/structures_tests.cpp)
    target_link_libraries(StructuresTests Affectation Clause Formula ExtendedFormula DPLL)
    target_link_libraries(StructuresTests cppunit)
    
    add_library(HeuristicsTests tests/heuristics_tests.cpp)
    target_link_libraries(HeuristicsTests Affectation Clause Formula ExtendedFormula DPLL)
    target_link_libraries(HeuristicsTests cppunit)

    add_library(DpllTests tests/dpll_tests.cpp)
    target_link_libraries(StructuresTests Affectation Clause Formula DPLL)
    target_link_libraries(StructuresTests cppunit)

    add_library(SatParserTests tests/satparser_tests.cpp)
    target_link_libraries(SatParserTests SatParser)
    target_link_libraries(SatParserTests cppunit)

    add_library(GraphParserTests tests/graphparser_tests.cpp)
    target_link_libraries(GraphParserTests GraphParser)
    target_link_libraries(GraphParserTests cppunit)

    add_library(GraphSolverTests tests/graphsolver_tests.cpp)
    target_link_libraries(GraphSolverTests GraphColorSolver)
    target_link_libraries(GraphSolverTests cppunit)

    add_executable(testrunner tests/main.cpp)
    target_link_libraries(testrunner StructuresTests HeuristicsTests DpllTests SatParserTests GraphParserTests GraphSolverTests)
    target_link_libraries(testrunner cppunit)
else (CPPUNIT_FOUND)
    message("+---------------------------------------------+")
    message("| CppUnit not found, tests won’t be compiled. |")
    message("+---------------------------------------------+")
endif (CPPUNIT_FOUND)
