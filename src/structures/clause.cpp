#include <cassert>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>

#include <set>
#include <unordered_set>
#include "clause.h"
#include "config.h"

using namespace satsolver;


Clause::Clause(int nb_var, std::vector<int> literals) {
    this->literals = std::unordered_set<int>() ;
    this->nb_variables = nb_var;
    for(std::vector<int>::iterator it = literals.begin(); it != literals.end(); ++it) {
        assert(*it != 0 && abs(*it) <= nb_var);
        this->literals.insert(*it) ;
    }
    if (WITH_WL)
        this->watched = std::pair<int,int>(0,0) ;
    this->aff = NULL ;
}

Clause::Clause(const Clause &c){
    this->literals = std::unordered_set<int>(c.literals) ;
    this->nb_variables = c.nb_variables ;
    if (WITH_WL)
        this->watched = std::pair<int,int>(c.watched) ;
    this->aff = c.aff ;
}

Clause& Clause::operator=(const Clause &that) {
    this->literals = that.literals;
    this->nb_variables = that.nb_variables;
    if (WITH_WL)
        this->watched = that.watched ;
    return *this;
    this->aff = that.aff ;
}

Clause::~Clause() {

}

int Clause::get_size() const {
    return (int) this->literals.size() ;
}

bool Clause::is_empty() const {
    return this->get_size() == 0 ;
}

bool Clause::contains_literal(int literal) const {
    assert(abs(literal)<=this->nb_variables && literal != 0);
    return this->literals.find(literal) != this->literals.end() ;
}

void Clause::add(int literal) {
    assert(abs(literal)<=this->nb_variables && literal != 0);
    this->literals.insert(literal) ;
}

void Clause::remove(int literal) {
    assert(abs(literal)<=this->nb_variables && literal != 0);
    this->literals.erase(literal) ;
}

bool Clause::is_tautology() const {
    for(int i = 1 ; i <= this->nb_variables ; i++) {
        if(this->contains_literal(i) && this->contains_literal(-i))
            return true ;
    }
    return false ;
}


std::string Clause::to_string()  {
    std::ostringstream oss;
    bool flag = false ;
    oss << "{" ;
    for(int i = 1 ; i <= this->nb_variables ; i++) {
        if(this->contains_literal(-i)) {
            if(flag)
                oss << "," ;
            flag = true ;
            oss << -i ;
        }
        if(this->contains_literal(i)) {
            if(flag)
                oss << "," ;
            flag = true ;
            oss << i ;
        }
    }
    oss << "}" ;
    if(WITH_WL) oss << " WL : " << this->fst_WL() << " & " << this->snd_WL()  ;
    return oss.str() ;
}

std::set<int> Clause::to_set() const {
    std::set<int> s = std::set<int>() ;
    for(auto l : this->literals) {
        if(!this->aff->is_false(l)) {
            s.insert(l) ;
        }
    }
    return s ;
}


bool Clause::contains_clause(satsolver::Clause &c) const {
    if(this->get_size() < c.get_size()) {
        return false ;
    }
    for(int i = 1 ; i <= this->nb_variables ; i++) {
        if(!this->contains_literal(i) && c.contains_literal(i))
            return false ;
        if(!this->contains_literal(-i) && c.contains_literal(-i))
            return false ;
    }
    return true ;
}

void Clause::init_WL() {
    assert(WITH_WL);
    assert(this->get_size() >= 2) ;
    std::unordered_set<int>::iterator it = this->literals.begin() ;
    this->watched.first = *it ;
    ++it ;
    this->watched.second = *it ;
}

int Clause::fst_WL() {
    assert(WITH_WL);
    return this->watched.first ;
}
int Clause::snd_WL() {
    assert(WITH_WL);
    return this->watched.second ;
}

bool Clause::is_WL(int x) {
    assert(WITH_WL);
    return this->watched.first == x || this->watched.second == x ;
}

void Clause::set_affectation(Affectation *a) {
    this->aff = a ;
}

int Clause::set_true(int x) {
    if (!WITH_WL)
        return 0;

    if(this->contains_literal(x) && !this->is_WL(x)) { // on installe x comme littéral surveillé
        if(this->aff->is_unknown(this->fst_WL()))  // priorité aux littéraux vrais
            this->watched.first = x ;
        else
            this->watched.second = x ;
        return 0 ;
    }
    else if(this->contains_literal(x)) { // rien
        return 0 ;
    }
    else if(this->is_WL(-x)) { // on surveille un littéral qui a été mis à faux
        if(this->aff->is_true(this->fst_WL()) || this->aff->is_true(this->snd_WL()))
            return 0 ; // rien à faire, la clause est satisfaite
        for(auto literal : this->literals) {
            if(!this->aff->is_false(literal) && !this->is_WL(literal)) { // nouveau literal surveillé
                if(this->fst_WL() == -x)
                    this->watched.first = literal ;
                else
                    this->watched.second = literal ;
                return 0 ; // pas de propagation unitaire, on a trouvé un remplaçant
            }
        }
        // On n'a pas trouvé de remplaçant, il ne reste donc qu'un literal non faux
        if(this->fst_WL() != -x)
            return this->fst_WL() ;
        else
            return this->snd_WL() ;
    }
    return 0 ;
}

int Clause::set_false(int x) {
    return this->set_true(-x) ;
}

bool Clause::is_true() {
    for(auto l : this->literals) {
        if(this->aff->is_true(l))
            return true ;
    }
    return false ;
}

int Clause::monome_begin() {
    if(this->literals.size() == 1)
        return *(this->literals.begin()) ;
    return 0 ;
}

int Clause::monome() {
    assert(!WITH_WL) ;
    int literal = 0, count = 0;
    for(auto l : literals) {
        if(this->aff->is_true(l))
            return 0 ;
        else if(this->aff->is_unknown(l)) {
            if(count > 0)
                return 0 ;
            literal = l ;
            count ++ ;
        }
    }
    return literal ;
}

bool Clause::is_evaluated_to_false() const {
    for (auto literal : this->literals)
        if (!this->aff->is_false(literal))
            return false;
    return true;
}

bool Clause::is_evaluated_to_true() const {
    for (auto literal : this->literals)
        if (this->aff->is_true(literal))
            return true;
    return false;
}

void Clause::add_literals_to_vector(std::vector<int> &v) const {
	for(auto l : this->literals) {
		if(this->aff->is_true(l)) // clause satisfaite, on sort
			return ;
	}
	for(auto l : this->literals) { // clause insatisfaite, on ajoute tous les littéraux non faux
		if(this->aff->is_unknown(l))
			v.push_back(l) ;
	}
}
