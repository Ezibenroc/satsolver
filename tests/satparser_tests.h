#ifndef SATPARSER_TESTS_H
#define SATPARSER_TESTS_H

#include <cppunit/TestFixture.h>

class SatParserTests : public CppUnit::TestFixture {
    private:
        void testBasic();
        void testExtraWhitespaces();
    public:
        static CppUnit::Test* suite();
};

#endif

