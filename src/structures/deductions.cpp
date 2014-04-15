#include <utility>
#include <iostream>
#include <stdexcept>

#include "structures/deductions.h"

Deductions::Deductions() : known_to_deduced(), deduced_to_known() {
}

bool Deductions::has_literal(int literal) const  {
    return this->deduced_to_known.find(literal) != deduced_to_known.end();
}
bool Deductions::has_variable(int var) const  {
    return this->deduced_to_known.find(var) != deduced_to_known.end() || deduced_to_known.find(-var) != deduced_to_known.end();
}

std::unordered_set<int> Deductions::get_deduced_from(int literal) const {
    try {
        return this->deduced_to_known.at(literal);
    }
    catch (std::out_of_range &e) {
        std::cout << "Error: Deductions::get_deduced_from with undefined literal." << std::endl;
        throw e;
    }
}

std::unordered_set<int> Deductions::get_deductions(int literal) const {
    try {
        return this->known_to_deduced.at(literal);
    }
    catch (std::out_of_range &e) {
        std::cout << "Error: Deductions::get_deductions with undefined literal." << std::endl;
        throw e;
    }
}

void Deductions::add_deduction(int literal, std::unordered_set<int> &clause) {
    this->deduced_to_known[literal] = clause;
    for (auto it : this->known_to_deduced) {
        auto pos = it.second.find(literal);
        if (pos != it.second.end())
            it.second.erase(pos);
        else {
            pos = it.second.find(-literal);
            if (pos != it.second.end())
                it.second.erase(pos);
        }
    }
    for (auto it : clause) {
        try {
            this->known_to_deduced.at(it).insert(literal);
        }
        catch (std::out_of_range) {
            this->known_to_deduced.insert(std::make_pair(it, std::unordered_set<int>({literal})));
        }
    }
}
void Deductions::add_deduction(int literal, std::set<int> clause) {
    std::unordered_set<int> clause2(clause.begin(), clause.end());
    this->add_deduction(literal, clause2);
}
