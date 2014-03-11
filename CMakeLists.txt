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
add_library(ExtendedFormula src/structures/extended_formula.cpp)
add_library(Graph src/structures/graph.cpp)

add_library(AbstractParser src/parsers/abstract.cpp)
add_library(SatParser src/parsers/sat.cpp)
target_link_libraries(SatParser AbstractParser Clause Formula)

add_library(GraphParser src/parsers/graph.cpp)
target_link_libraries(GraphParser AbstractParser Graph)

add_library(DPLL src/solvers/dpll.cpp)
target_link_libraries(DPLL Affectation Clause Formula)

add_library(ResolCommon src/entry_points/resol_common.cpp)
target_link_libraries(ResolCommon Affectation Formula)
target_link_libraries(ResolCommon SatParser)
target_link_libraries(ResolCommon DPLL)

add_executable(resol src/entry_points/resol.cpp)
target_link_libraries(resol ResolCommon)

add_executable(resol-wl src/entry_points/resol-wl.cpp)
target_link_libraries(resol-wl ResolCommon)

find_package(CppUnit)
if (CPPUNIT_FOUND)
    add_library(StructuresTests tests/structures_tests.cpp)
    target_link_libraries(StructuresTests Affectation Clause Formula Graph)
    target_link_libraries(StructuresTests cppunit)

    add_library(DpllTests tests/dpll_tests.cpp)
    target_link_libraries(StructuresTests Affectation Clause Formula DPLL)
    target_link_libraries(StructuresTests cppunit)

    add_library(SatParserTests tests/satparser_tests.cpp)
    target_link_libraries(SatParserTests SatParser)
    target_link_libraries(SatParserTests cppunit)

    add_library(GraphParserTests tests/graphparser_tests.cpp)
    target_link_libraries(GraphParserTests GraphParser)
    target_link_libraries(GraphParserTests cppunit)

    add_executable(testrunner tests/main.cpp)
    target_link_libraries(testrunner StructuresTests DpllTests SatParserTests GraphParserTests)
    target_link_libraries(testrunner cppunit)
else (CPPUNIT_FOUND)
    message("+---------------------------------------------+")
    message("| CppUnit not found, tests won’t be compiled. |")
    message("+---------------------------------------------+")
endif (CPPUNIT_FOUND)
