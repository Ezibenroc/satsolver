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
    CPPUNIT_ASSERT_NO_THROW(satsolver::Parser parser(stream2));
}
void ParserTests::testExtraWhitespaces() {
}


CppUnit::Test* ParserTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("ParserTests");
    suite->addTest(new CppUnit::TestCaller<ParserTests>("testBasic",
                &ParserTests::testBasic));
    suite->addTest(new CppUnit::TestCaller<ParserTests>("testExtraWhitespaces",
                &ParserTests::testExtraWhitespaces));
    return suite;
}
