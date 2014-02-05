cmake_minimum_required (VERSION 2.6)
project(satsolver)

add_definitions(-std=c++0x)
add_definitions(-g)

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)


add_library(Structures src/structures.cpp)
include_directories ("${PROJECT_SOURCE_DIR}")

add_executable(main src/main.cpp)
target_link_libraries(main Structures)

MESSAGE("${CMAKE_MODULE_PATH}")

find_package(CppUnit)
if (CPPUNIT_FOUND)
    add_library(StructuresTests tests/structures_tests.cpp)
    target_link_libraries(StructuresTests Structures)
    target_link_libraries(StructuresTests cppunit)

    add_executable(testrunner tests/main.cpp)
    target_link_libraries(testrunner StructuresTests)
    target_link_libraries(testrunner cppunit)
endif (CPPUNIT_FOUND)
