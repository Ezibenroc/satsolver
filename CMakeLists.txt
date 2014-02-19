cmake_minimum_required (VERSION 2.6)
project(satsolver)

add_definitions(-std=c++0x)
add_definitions(-g -Wall -Wextra -pedantic -fmax-errors=1)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

include_directories ("${PROJECT_SOURCE_DIR}")

add_library(Affectation src/structures/affectation.cpp)
add_library(Clause src/structures/clause.cpp)
add_library(Formula src/structures/formula.cpp)
add_library(Parser src/parser.cpp)

add_executable(main src/main.cpp)
target_link_libraries(main Affectation Clause Formula)
target_link_libraries(main Parser)

find_package(CppUnit)
if (CPPUNIT_FOUND)
    add_library(StructuresTests tests/structures_tests.cpp)
    target_link_libraries(StructuresTests Affectation Clause Formula)
    target_link_libraries(StructuresTests cppunit)

    add_library(ParserTests tests/parser_tests.cpp)
    target_link_libraries(ParserTests Parser)
    target_link_libraries(ParserTests cppunit)

    add_executable(testrunner tests/main.cpp)
    target_link_libraries(testrunner StructuresTests)
    target_link_libraries(testrunner ParserTests)
    target_link_libraries(testrunner cppunit)
endif (CPPUNIT_FOUND)
