#include <cassert>
#include <iostream>
#include <algorithm>

#include "structures/extended_formula.h"

using namespace satsolver;

#define EF ExtendedFormula
#define SPEF std::shared_ptr<EF>

#define remove_minus(str) ((str[0] == '-' ? str.erase(str.begin()+1, str.end())), str)

int EF::last_id = 0;

EF::EF(const EF *that) : type(that->type), f1(that->f1), f2(that->f2), literal(that->literal), id(++this->last_id) {
}

EF::EF(EF::Type type) : type (type), id(++this->last_id) {
    assert(type == EF::TRUE || type == EF::FALSE);
}

EF::EF(EF::Type type, std::string literal) : type (type), literal(literal), id(++this->last_id) {
    assert(type == EF::LITERAL);
}

EF::EF(EF::Type type, SPEF f) : type (type), f1(f), id(++this->last_id) {
    assert(type == EF::NOT);;
}

EF::EF(EF::Type type, SPEF f1, SPEF f2) : type (type), f1(f1), f2(f2), id(++this->last_id) {
    assert(type != EF::TRUE);
    assert(type != EF::FALSE);
    assert(type != EF::LITERAL);
    assert(type != EF::NOT);
}

std::string EF::make_literal() const {
    if (this->type == EF::LITERAL)
        return this->literal;
    else
        return std::string("@") + std::to_string(this->id);
}

std::vector<std::vector<std::string>*>* EF::reduce() const {
    std::vector<std::vector<std::string>*> *result = new std::vector<std::vector<std::string>*>(), *f1_reduction=NULL, *f2_reduction=NULL;
    std::vector<std::string> *clause;

    if (type == EF::LITERAL || type == EF::TRUE || type == EF::FALSE) {
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
                    f1->make_literal(),
                    f2->make_literal(),
                    std::string("-") + this->make_literal()
                };
            result->push_back(clause); // A v B v -C
            clause = new std::vector<std::string>{
                    std::string("-") + f1->make_literal(),
                    this->make_literal(),
                    ""
                };
            result->push_back(clause); // -A v C
            clause = new std::vector<std::string>{
                    std::string("-") + f2->make_literal(),
                    this->make_literal(),
                    ""
                };
            result->push_back(clause); // -B v C
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
            result->push_back(clause); // -A v -B
            clause = new std::vector<std::string>{
                    f1->make_literal(),
                    this->make_literal(),
                    ""
                };
            result->push_back(clause); // A v B
            break;
        case LITERAL:
        case TRUE:
        case FALSE:
            break;
    }
    if (f1_reduction)
        delete f1_reduction;
    if (f2_reduction)
        delete f2_reduction;
    return result;
}


std::shared_ptr<Formula> EF::reduce_to_formula(std::shared_ptr<std::map<std::string, int>> *name_to_variable_ptr) const {
    std::vector<std::vector<std::string>*> *raw_clauses = this->reduce();
    std::shared_ptr<std::map<std::string, int>> name_to_variable = std::make_shared<std::map<std::string, int>>();
    int nb_variables = 0;
    int i;
    std::vector<std::shared_ptr<satsolver::Clause>> clauses;
    std::shared_ptr<std::vector<int>> int_literals;
    std::string string_literal;

    raw_clauses->push_back(new std::vector<std::string>{this->make_literal(), "", ""});

    for (auto raw_clause : *raw_clauses) {
        for (i=0; i<3; i++) {
            string_literal = raw_clause->at(i);
            if (string_literal[0] == '-')
                string_literal.erase(string_literal.begin(), string_literal.begin()+1);
            if (string_literal != "" && name_to_variable->find(string_literal) == name_to_variable->end()) {
                name_to_variable->insert(std::make_pair(string_literal, ++nb_variables));
            }
        }
    }

    for (auto raw_clause : *raw_clauses) {
        int_literals = std::make_shared<std::vector<int>>();
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
        assert(int_literals->size());
        clauses.push_back(std::shared_ptr<satsolver::Clause>(new satsolver::Clause(nb_variables, int_literals)));
        delete raw_clause;
    }
    delete raw_clauses;
    if (clauses.size() == 1) // Conjonction of zero clauses
        return NULL;
    
    if (name_to_variable_ptr)
        *name_to_variable_ptr = name_to_variable;
    return std::shared_ptr<Formula>(new Formula(clauses, nb_variables));


}

std::string EF::to_string() const {
    switch (this->type) {
        case EF::XOR:
            return "(" + this->f1->to_string() + "⊕" + this->f2->to_string() + ")";
        case EF::OR:
            return "(" + this->f1->to_string() + "+" + this->f2->to_string() + ")";
        case EF::AND:
            return "(" + this->f1->to_string() + "•" + this->f2->to_string() + ")";
        case EF::IMPLIES:
            return "(" + this->f1->to_string() + "→" + this->f2->to_string() + ")";
        case EF::NOT:
            return "(¬" + this->f1->to_string() + ")";
        case EF::LITERAL:
            return "“" + this->literal + "”";
        case EF::TRUE:
            return "⊤";
        case EF::FALSE:
            return "⊥";
    }
    assert(false);
}

bool EF::operator==(const ExtendedFormula &that) const {
    if (this->type != that.type) {
        return false;
    }
    switch (this->type) {
        case EF::OR:
        case EF::XOR:
        case EF::AND:
            return ((*this->f1 == *that.f1 && *this->f2 == *that.f2) || (*this->f1 == *that.f2 && *this->f2 == *that.f1));
        case EF::IMPLIES:
            return (*this->f1 == *that.f1 && *this->f2 == *that.f2);
        case EF::NOT:
            return *this->f1 == *that.f1;
        case EF::LITERAL:
            return this->literal == that.literal;
        case TRUE:
        case FALSE:
            return true;
    }
    assert(false);
}

bool EF::operator!=(const ExtendedFormula &that) const {
    return !(*this == that);
}

SPEF EF::simplify() const {
    SPEF f1, f2;
    switch(this->type) {
        case EF::OR:
        case EF::AND:
        case EF::XOR:
        case EF::IMPLIES:
            f1 = this->f1->simplify();
            f2 = this->f2->simplify();
            break;
        case EF::NOT:
            f1 = this->f1->simplify();
            break;
        default:
            break;
    }
    switch (this->type) {
        case EF::OR:
            if (f1->type == EF::TRUE)
                return f1;
            else if (f2->type == EF::TRUE)
                return f2;
            else if (*f1 == EF(EF::NOT, f2))
                return SPEF(new EF(EF::TRUE));
            else if (*f2 == EF(EF::NOT, f1))
                return SPEF(new EF(EF::TRUE));
            else if (f1->type == EF::FALSE)
                return SPEF(new EF(*f2));
            else if (f2->type == EF::FALSE)
                return SPEF(new EF(*f1));
            break;
        case EF::AND:
            if (f1->type == EF::FALSE)
                return f1;
            else if (f2->type == EF::FALSE)
                return f2;
            else if (*f1 == *f2)
                return SPEF(new EF(*f1));
            else if (f1->type == TRUE)
                return SPEF(new EF(*f2));
            else if (f2->type == TRUE)
                return SPEF(new EF(*f1));
            if (*f1 == EF(EF::NOT, f2))
                return SPEF(new EF(EF::FALSE));
            else if (*f2 == EF(EF::NOT, f1))
                return SPEF(new EF(EF::FALSE));
            break;
        default:
            break;
    }
    switch(this->type) {
        case EF::OR:
        case EF::AND:
        case EF::XOR:
        case EF::IMPLIES:
            return SPEF(new EF(this->type, f1, f2));
        case EF::NOT:
            if (f1->type == EF::NOT)
                return SPEF(new EF(*f1->f1));
            else
                return SPEF(new EF(this->type, f1));
        default:
            return SPEF(new EF(this));
    }
    assert(false);
}
