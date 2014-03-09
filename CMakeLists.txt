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

add_library(AbstractParser src/parsers/abstract.cpp)
add_library(SatParser src/parsers/sat.cpp)
target_link_libraries(SatParser AbstractParser Clause Formula)

add_library(DPLL src/dpll.cpp)
target_link_libraries(DPLL Affectation Clause Formula)

add_library(solver_main src/solver_main.cpp)
target_link_libraries(solver_main Affectation Formula)
target_link_libraries(solver_main SatParser)
target_link_libraries(solver_main DPLL)

add_executable(resol src/entry_points/resol.cpp)
target_link_libraries(resol solver_main)

add_executable(resol-wl src/entry_points/resol-wl.cpp)
target_link_libraries(resol-wl solver_main)

find_package(CppUnit)
if (CPPUNIT_FOUND)
    add_library(StructuresTests tests/structures_tests.cpp)
    target_link_libraries(StructuresTests Affectation Clause Formula)
    target_link_libraries(StructuresTests cppunit)

    add_library(DpllTests tests/dpll_tests.cpp)
    target_link_libraries(StructuresTests Affectation Clause Formula DPLL)
    target_link_libraries(StructuresTests cppunit)

    add_library(SatParserTests tests/satparser_tests.cpp)
    target_link_libraries(SatParserTests SatParser)
    target_link_libraries(SatParserTests cppunit)

    add_executable(testrunner tests/main.cpp)
    target_link_libraries(testrunner StructuresTests)
    target_link_libraries(testrunner DpllTests)
    target_link_libraries(testrunner SatParserTests)
    target_link_libraries(testrunner cppunit)
else (CPPUNIT_FOUND)
    message("+---------------------------------------------+")
    message("| CppUnit not found, tests won’t be compiled. |")
    message("+---------------------------------------------+")
endif (CPPUNIT_FOUND)
