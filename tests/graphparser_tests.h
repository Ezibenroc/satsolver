#ifndef GRAPHPARSER_TESTS_H
#define GRAPHPARSER_TESTS_H

#include <cppunit/TestFixture.h>

class GraphParserTests : public CppUnit::TestFixture {
    private:
        void testBasic();
        void testExtraWhitespaces();
    public:
        static CppUnit::Test* suite();
};

#endif


