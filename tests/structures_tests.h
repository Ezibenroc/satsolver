#ifndef STRUCTURES_TESTS_H
#define STRUCTURES_TESTS_H

#include <cppunit/TestFixture.h>
class StructuresTests : public CppUnit::TestFixture {
    private:
        void testClauseCreation();
        void testBasicUsage();
        void testAffectationCreationUsage();
        void testSetTrueClause() ;
				void testFormula() ;
/*        void testUnitPropagation() ;
        void testIsolatedLiterals() ;
        void testClean() ;*/
    public:
        static CppUnit::Test* suite();
};

#endif
