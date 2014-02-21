#ifndef STRUCTURES_TESTS_H
#define STRUCTURES_TESTS_H

#include <cppunit/TestFixture.h>
class StructuresTests : public CppUnit::TestFixture {
    private:
        void testClauseCreation();
        void testBasicUsage();
        void testAffectationCreationUsage();
				void testFormula() ;
        void testUnitPropagation() ;
        void testIsolatedLiterals() ;
    public:
        static CppUnit::Test* suite();
};

#endif
