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
    return this->deduced_to_known.at(literal);
}

std::unordered_set<int> Deductions::get_deductions(int literal) const {
    return this->known_to_deduced.at(literal);
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

void Deductions::remove_unknown(satsolver::Affectation &aff) {
    std::unordered_set<int> to_remove;
    for (auto pos=this->known_to_deduced.begin(); pos != this->known_to_deduced.end(); pos++) {
        if (aff.is_unknown(pos->first)) {
            to_remove.insert(pos->first);
        }
    }
    for (auto it : to_remove) {
        this->known_to_deduced.erase(it);
        for (auto it2 : this->deduced_to_known) {
            it2.second.erase(it);
        }
    }
}


void Deductions::print() {
	for(auto it : this->deduced_to_known) {
		std::cout << it.first << " deduced with " ;
		for(auto l : it.second) {
			std::cout << l << " " ;
		}
		std::cout << std::endl ;
	}
}
