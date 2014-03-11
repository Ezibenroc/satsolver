#include <cassert>
#include <iostream>
#include <algorithm>

#include "structures/extended_formula.h"

using namespace satsolver;

#define EF ExtendedFormula
#define SPEF std::shared_ptr<EF>

#define remove_minus(str) ((str[0] == '-' ? str.erase(str.begin()+1, str.end())), str)

int EF::last_id = 0;

EF::EF(EF::Type type, std::string literal) : type (type), literal(literal), id(++this->last_id) {
    assert(type == EF::LITERAL);
}

EF::EF(EF::Type type, SPEF f) : type (type), f1(f), id(++this->last_id) {
    assert(type == EF::NOT);;
}

EF::EF(EF::Type type, SPEF f1, SPEF f2) : type (type), f1(f1), f2(f2), id(++this->last_id) {
    assert(type != EF::LITERAL);
    assert(type != EF::NOT);
}

std::string EF::make_literal() {
    return std::string("@") + std::to_string(this->id);
}

std::vector<std::vector<std::string>*>* EF::reduce() {
    std::vector<std::vector<std::string>*> *result = new std::vector<std::vector<std::string>*>(), *f1_reduction, *f2_reduction;
    std::vector<std::string> *clause;

    if (type == EF::LITERAL) {
    }
    else if (type == EF::NOT) {
        f1_reduction = this->f1->reduce();
        result->reserve(f1_reduction->size() + 2);
        result->insert(result->end(), f1_reduction->begin(), f1_reduction->end());
    }
    else {
        f1_reduction = this->f1->reduce();
        f2_reduction = this->f2->reduce();
        result->reserve(f1_reduction->size() + f2_reduction->size() + 4);
        result->insert(result->end(), f1_reduction->begin(), f1_reduction->end());
        result->insert(result->end(), f2_reduction->begin(), f2_reduction->end());
    }

    switch (this->type) {
        case AND:
            clause = new std::vector<std::string>{
                    std::string("-") + f1->make_literal(),
                    std::string("-") + f2->make_literal(),
                    this->make_literal()
                };
            result->push_back(clause); // -A v -B v C
            clause = new std::vector<std::string>{
                    f1->make_literal(),
                    std::string("-") + this->make_literal(),
                    ""
                };
            result->push_back(clause); // A v -C
            clause = new std::vector<std::string>{
                    f2->make_literal(),
                    std::string("-") + this->make_literal(),
                    ""
                };
            result->push_back(clause); // B v -C
            break;
        case OR:
            clause = new std::vector<std::string>{
                    std::string("-") + f1->make_literal(),
                    std::string("-") + f2->make_literal(),
                    std::string("-") + this->make_literal()
                };
            result->push_back(clause); // -A v -B v -C
            clause = new std::vector<std::string>{
                    f1->make_literal(),
                    this->make_literal(),
                    ""
                };
            result->push_back(clause); // A v C
            clause = new std::vector<std::string>{
                    f2->make_literal(),
                    this->make_literal(),
                    ""
                };
            result->push_back(clause); // B v C
            break;
        case XOR:
            clause = new std::vector<std::string>{
                    std::string("-") + f1->make_literal(),
                    std::string("-") + f2->make_literal(),
                    std::string("-") + this->make_literal()
                };
            result->push_back(clause); // -A v -B v -C
            clause = new std::vector<std::string>{
                    f1->make_literal(),
                    f2->make_literal(),
                    std::string("-") + this->make_literal()
                };
            result->push_back(clause); // A v B v -C
            clause = new std::vector<std::string>{
                    f1->make_literal(),
                    std::string("-") + f2->make_literal(),
                    this->make_literal()
                };
            result->push_back(clause); // A v -B v C
            clause = new std::vector<std::string>{
                    std::string("-") + f1->make_literal(),
                    f2->make_literal(),
                    this->make_literal()
                };
            result->push_back(clause); // -A v B v C
            break;
        case NOT:
            clause = new std::vector<std::string>{
                    std::string("-") + f1->make_literal(),
                    std::string("-") + this->make_literal(),
                    ""
                };
            result->push_back(clause); // A v -B v C
            clause = new std::vector<std::string>{
                    f1->make_literal(),
                    this->make_literal(),
                    ""
                };
            result->push_back(clause); // -A v B v C
            break;
        case LITERAL:
            clause = new std::vector<std::string>{this->literal, "", ""};
            result->push_back(clause);
            break;
    }
    return result;
}


std::shared_ptr<Formula> EF::reduce_to_formula(std::shared_ptr<std::map<std::string, int>> *name_to_variable_ptr) {
    std::vector<std::vector<std::string>*> *raw_clauses = this->reduce();
    std::shared_ptr<std::map<std::string, int>> name_to_variable = std::make_shared<std::map<std::string, int>>();
    int nb_variables = 0;
    int i;
    std::vector<std::shared_ptr<satsolver::Clause>> clauses;
    std::vector<int> *int_literals;
    std::string string_literal;

    for (auto raw_clause : *raw_clauses) {
        for (i=0; i<3; i++) {
            string_literal = raw_clause->at(i);
            if (string_literal[0] == '-')
                string_literal.erase(string_literal.begin(), string_literal.begin()+1);
            if (string_literal != "" && name_to_variable->find(string_literal) == name_to_variable->end())
                name_to_variable->insert(std::make_pair(string_literal, ++nb_variables));
        }
    }

    for (auto raw_clause : *raw_clauses) {
        int_literals = new std::vector<int>();
        for (i=0; i<3; i++) {
            string_literal = raw_clause->at(i);
            if (string_literal[0] == '-') {
                string_literal.erase(string_literal.begin(), string_literal.begin()+1);
                int_literals->push_back(- name_to_variable->at(string_literal));
            }
            else if (string_literal[0]) {
                int_literals->push_back(name_to_variable->at(string_literal));
            }
        }
        clauses.push_back(std::shared_ptr<satsolver::Clause>(new satsolver::Clause(nb_variables, *int_literals)));
    }
    
    if (name_to_variable_ptr)
        *name_to_variable_ptr = name_to_variable;
    return std::shared_ptr<Formula>(new Formula(clauses, nb_variables));


}
