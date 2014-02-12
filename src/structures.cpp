#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
    this->nb_unknown = this->size ;
    this->nb_true = 0 ;
    this->nb_false = 0 ;
}

Clause::Clause(int nb_var, std::vector<int> literals) {
    int sub ;
    this->literals = (bool*) malloc(nb_var*2);
    this->size = 0 ;
    this->nb_variables = nb_var;
    memset(this->literals, false, nb_var*2);
    for(std::vector<int>::iterator it = literals.begin(); it != literals.end(); ++it) {
        assert(*it != 0);
        if(*it > 0)
            sub = 1 ;
        else
            sub = 0 ;
        this->literals[nb_var + *it - sub] = true;
        this->size ++ ;
    }
    this->nb_unknown = this->size ;
    this->nb_true = 0 ;
    this->nb_false = 0 ;
}

Clause::Clause(const Clause &c){
    this->literals = (bool*) malloc(c.nb_variables*2);
    this->nb_variables = c.nb_variables ;
    this->size = c.size ;
    for(int i = 0 ; i < this->nb_variables*2 ; i++) {
    	this->literals[i] = c.literals[i] ;
    }
    this->nb_unknown = c.nb_unknown ;
    this->nb_true = c.nb_true ;
    this->nb_false = c.nb_false ;
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
        this->nb_unknown ++ ;
        if(literal > 0)
            sub = 1;
        else
            sub = 0;
        this->literals[this->nb_variables + literal - sub] = true;
    }
}	

/* Ne pas utlisier : problème de mis à jour de nb_unknown, nb_true, nb_false
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
*/

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


bool Clause::is_true() {
	return this->nb_true > 0 ;
}
bool Clause::is_false() {
	return this->nb_false == this->size ;
}
int Clause::monome(Affectation a) {
	if(this->nb_unknown != 1 || this->nb_true > 0)
		return 0 ;
	for(int i = 1 ; i <= this->nb_variables ; i++) {
		if(this->contains_literal(i) && a.is_unknown(i)) {
			return i ;
		}
		if(this->contains_literal(-i) && a.is_unknown(-i)) {
			return -i ;
		}
	}
	std::cout << "Error in clause::monome." << std::endl ;
	exit(EXIT_FAILURE) ;
}

Affectation::Affectation(int nb_var) {
	std::vector<int> t ;
	this->nb_aff = 0 ;
	this->aff = t ;
	for(int i = 0 ; i < nb_var ; i++)
		this->aff.push_back(0) ;
}

bool Affectation::is_true(int x) {
	assert(x > 0 && x <= (int) this->aff.size()) ;
	return this->aff[x-1] == 1 ;
}
bool Affectation::is_false(int x) {
	assert(x > 0 && x <= (int) this->aff.size()) ;
	return this->aff[x-1] == -1 ;
}
bool Affectation::is_unknown(int x) {
	assert(x > 0 && x <= (int) this->aff.size()) ;
	return this->aff[x-1] == 0 ;
}

void Affectation::set_true(int x) {
	assert(x > 0 && x <= (int) this->aff.size()) ;
	this->aff[x-1] = 1 ;
}
void Affectation::set_false(int x) {
	assert(x > 0 && x <= (int) this->aff.size()) ;
	this->aff[x-1] = -1 ;
}
void Affectation::set_unknown(int x) {
	assert(x > 0 && x <= (int) this->aff.size()) ;
	this->aff[x-1] = 0 ;
}
