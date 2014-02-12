#ifndef PARSER_TESTS_H
#define PARSER_TESTS_H

#include <cppunit/TestFixture.h>

class ParserTests : public CppUnit::TestFixture {
    private:
        void testBasic();
        void testExtraWhitespaces();
    public:
        static CppUnit::Test* suite();
};

#endif

