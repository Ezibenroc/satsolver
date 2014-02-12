#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "structures.h"

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

Clause::Clause(int nb_variables, std::vector<int> literals) {
    int sub ;
    this->literals = (bool*) malloc(nb_variables*2);
    nb_variables = nb_variables;
    memset(this->literals, false, nb_variables*2);
    for(std::vector<int>::iterator it = literals.begin(); it != literals.end(); ++it) {
        assert(*it != 0);
        if(*it > 0)
            sub = 1 ;
        else
            sub = 0 ;
        this->literals[nb_variables + *it - sub] = true;
    }
}

Clause::Clause(const Clause &c){
    this->literals = (bool*) malloc(c.nb_variables*2);
    this->nb_variables = c.nb_variables ;
    this->size = c.size ;
    for(int i = 0 ; i < this->nb_variables*2 ; i++) {
    	this->literals[i] = c.literals[i] ;
    }
}

bool Clause::contains_literal(int literal) {
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
        this->literals[this->nb_variables + literal - sub] = false;
    }
}

/* A PRIORI INUTILE : pas d'union dans DPLL
Clause* Clause::disjonction(Clause *clause2) {
    assert(nb_variables == clause2->nb_variables);
    bool *new_literals = (bool*) malloc(nb_variables*2);
    for (int i=0; i<nb_variables*2; i++)
        new_literals[i] = literals[i] || clause2->literals[i];
    return new Clause(nb_variables, new_literals);
}*/

std::string Clause::to_string() {
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
	return oss.str() ;
}

std::set<int> Clause::to_set() {
    std::set<int> set;
	for(int i = 1 ; i <= this->nb_variables ; i++) {
		if(this->contains_literal(-i))
            set.insert(-i);
		if(this->contains_literal(i))
            set.insert(i);
    }
    return set;
}
