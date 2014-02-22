#include <cassert>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>

#include "clause.h"

using namespace satsolver;

Clause::Clause(int nb_variables, bool *literals) {
    this->literals = literals;
    this->nb_variables = nb_variables ;
    this->size = 0 ;
    for(int i = 0 ; i < nb_variables*2 ; i++) {
        if(literals[i])
            this->size ++ ;
    }
}

Clause::Clause(int nb_var, std::vector<int> literals) {
    int sub ;
    this->literals = (bool*) malloc(sizeof(bool)*nb_var*2);
    this->size = 0 ;
    this->nb_variables = nb_var;
    memset(this->literals, false, nb_var*2);
    for(std::vector<int>::iterator it = literals.begin(); it != literals.end(); ++it) {
        assert(*it != 0);
        if(*it > 0)
            sub = 1 ;
        else
            sub = 0 ;
        if (!this->literals[nb_var + *it - sub]) {
            this->literals[nb_var + *it - sub] = true;
            this->size ++ ;
        }
    }
}

Clause::Clause(const Clause &c){
    this->literals = (bool*) malloc(sizeof(bool)*c.nb_variables*2);
    this->nb_variables = c.nb_variables ;
    this->size = 0;
    for(int i = 0 ; i < this->nb_variables*2 ; i++) {
        this->literals[i] = c.literals[i];
        if (c.literals[i])
            this->size++;
    }
    assert(this->size == c.size);
}

Clause& Clause::operator=(const Clause &that) {
    this->literals = that.literals;
    this->nb_variables = that.nb_variables;
    this->size = that.size;
    return *this;
}

Clause::~Clause() {
    free(this->literals);
}

int Clause::get_size() const {
    return this->size ;
}

bool Clause::is_empty() const {
    return this->size == 0 ;
}

bool Clause::contains_literal(int literal) const {
    int sub ;
    assert(literal>=-nb_variables);
    assert(literal<=nb_variables);
    if(literal > 0)
        sub = 1;
    else
        sub = 0;
    return this->literals[this->nb_variables + literal - sub];
}

void Clause::add(int literal) {
    int sub;
    if(!this->contains_literal(literal)) {
        this->size ++;
        if(literal > 0)
            sub = 1;
        else
            sub = 0;
        assert(!this->literals[this->nb_variables + literal - sub]);
        this->literals[this->nb_variables + literal - sub] = true;
    }
}

void Clause::remove(int literal) {
    int sub ;
    if(this->contains_literal(literal)) {
        this->size -- ;
        if(literal > 0)
            sub = 1;
        else
            sub = 0;
        assert(this->literals[this->nb_variables + literal - sub]);
        this->literals[this->nb_variables + literal - sub] = false;
    }
    if(this->size == 0)
        throw Conflict() ;
}

bool Clause::is_tautology() const {
        for(int i = 1 ; i <= this->nb_variables ; i++) {
            if(this->contains_literal(i) && this->contains_literal(-i))
                return true ;
        }
        return false ;
}    


std::string Clause::to_string() const {
    std::ostringstream oss;
    bool flag = false ;
    for(int i = 1 ; i <= this->nb_variables ; i++) {
        if(this->contains_literal(-i)) {
            if(flag)
                oss << " " ;
            flag = true ;
            oss << -i ;
        }
        if(this->contains_literal(i)) {
            if(flag)
                oss << " " ;
            flag = true ;
            oss << i ;
        }
    }
    return oss.str() ;
}

std::set<int> Clause::to_set() const {
  std::set<int> set;
    for(int i = 1 ; i <= this->nb_variables ; i++) {
        if(this->contains_literal(-i))
            set.insert(-i);
        if(this->contains_literal(i))
            set.insert(i);
    }
    return set;
}


int Clause::monome() const {
    if(this->size != 1)
        return 0 ;
    for(int i = 1 ; i <= this->nb_variables ; i++) {
        if(this->contains_literal(i)) {
            return i ;
        }
        if(this->contains_literal(-i)) {
            return -i ;
        }
    }
    std::cout << "Error in clause::monome." << std::endl ;
    exit(EXIT_FAILURE) ;
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
