#include <climits>

#include "structures/extended_formula.h"
#include "structures/difference_atom.h"
#include "solvers/difference_assistant.h"
#include "solvers/tseitin.h"
#include "theory_tests.h"

#define EF satsolver::ExtendedFormula
#define SPEF std::shared_ptr<EF>
#define DA theorysolver::DifferenceAtom
#define SPDA std::shared_ptr<DA>

#define INIT  SPEF ef;\
    std::shared_ptr<satsolver::Formula> f;\
    std::shared_ptr<std::map<std::string, int>> name_to_variable;\
    std::vector<SPDA> literal_to_DA;\
    std::vector<unsigned int> affected_literals;

void TheoryTests::testDifferencesObviousFail() {
    INIT;
    theorysolver::DifferenceAssistant *assistant;

    ef = SPEF(new EF(EF::AND,
                SPEF(new EF(EF::LITERAL, "#1")),
                SPEF(new EF(EF::LITERAL, "#2"))
            ));
    literal_to_DA.push_back(SPDA(new DA(1, 2, DA::LOWER, 2)));
    literal_to_DA.push_back(SPDA(new DA(1, 2, DA::GREATER, 2)));
    ef = theorysolver::DifferenceAssistant::canonize_formula(ef, literal_to_DA);
    CPPUNIT_ASSERT(tseitin_reduction(false, ef, name_to_variable, f, &affected_literals));
    assistant = new theorysolver::DifferenceAssistant(literal_to_DA, name_to_variable, f);
    CPPUNIT_ASSERT(-1 == assistant->on_flip(name_to_variable->at("#3")));
    CPPUNIT_ASSERT(-1 != assistant->on_flip(name_to_variable->at("#4")));
}

void TheoryTests::testDifferencesLessObviousFail() {
    INIT;
    theorysolver::DifferenceAssistant *assistant;

    ef = SPEF(new EF(EF::AND,
                SPEF(new EF(EF::LITERAL, "#1")),
                SPEF(new EF(EF::OR,
                        SPEF(new EF(EF::LITERAL, "#2")),
                        SPEF(new EF(EF::LITERAL, "#3"))
                    ))
            ));
    literal_to_DA.push_back(SPDA(new DA(1, 2, DA::LOWER, 2)));
    literal_to_DA.push_back(SPDA(new DA(1, 2, DA::GREATER, 2)));
    literal_to_DA.push_back(SPDA(new DA(1, 2, DA::GREATER, 2)));
    ef = theorysolver::DifferenceAssistant::canonize_formula(ef, literal_to_DA);
    CPPUNIT_ASSERT(tseitin_reduction(false, ef, name_to_variable, f, &affected_literals));
    assistant = new theorysolver::DifferenceAssistant(literal_to_DA, name_to_variable, f);
    if (f->get_aff()->is_unknown(name_to_variable->at("#4")))
        f->get_aff()->set_true(name_to_variable->at("#4"));
    CPPUNIT_ASSERT(-1 == assistant->on_flip(name_to_variable->at("#4")));
    CPPUNIT_ASSERT(assistant->is_state_consistent());
    if (f->get_aff()->is_unknown(name_to_variable->at("#5")))
        f->get_aff()->set_false(name_to_variable->at("#5"));
    if (f->get_aff()->is_unknown(name_to_variable->at("#6")))
        f->get_aff()->set_true(name_to_variable->at("#6"));
    CPPUNIT_ASSERT(-1 != assistant->on_flip(name_to_variable->at("#5")) || -1 != assistant->on_flip(name_to_variable->at("#6")));
    CPPUNIT_ASSERT(!assistant->is_state_consistent());

    f->get_aff()->set_unknown(name_to_variable->at("#5"));
    f->get_aff()->set_unknown(name_to_variable->at("#6"));
    CPPUNIT_ASSERT(-1 == assistant->on_flip(name_to_variable->at("#5")) || -1 == assistant->on_flip(name_to_variable->at("#6")));
    CPPUNIT_ASSERT(assistant->is_state_consistent());
}

CppUnit::Test* TheoryTests::suite() {
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("TheoryTests");
    suite->addTest(new CppUnit::TestCaller<TheoryTests>("TheoryTests_testDifferencesObviousFail",
                &TheoryTests::testDifferencesObviousFail));
    suite->addTest(new CppUnit::TestCaller<TheoryTests>("TheoryTests_testDifferencesLessObviousFail",
                &TheoryTests::testDifferencesLessObviousFail));
    return suite;
}
