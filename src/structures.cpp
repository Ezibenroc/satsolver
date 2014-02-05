#include <cassert>
#include <cstdlib>
#include <cstring>

#include "structures.h"

using namespace satsolver;

Clause::Clause(int nb_variables, bool *literals) {
    this->m_literals = literals;
    this->m_nb_variables = nb_variables ;
    this->m_size = 0 ;
    for(int i = 0 ; i < nb_variables*2 ; i++) {
    	if(literals[i])
    		this->m_size ++ ;
    }
}

/*
Clause::Clause(int nb_variables, std::vector<int> literals) {
    m_literals = (bool*) malloc(nb_variables*2 + 1);
    memset(m_literals, false, nb_variables*2 + 1);
    for(std::vector<int>::iterator it = literals.begin(); it != literals.end(); ++it) {
        m_literals[*it] = true;
    }
}*/

Clause::Clause(int nb_variables, std::vector<int> literals) {
		int sub ;
    this->m_literals = (bool*) malloc(nb_variables*2);
    memset(this->m_literals, false, nb_variables*2);
    for(std::vector<int>::iterator it = literals.begin(); it != literals.end(); ++it) {
    		assert(*it != 0) ;
    		if(*it > 0) 
    			sub = 1 ;
    		else
    			sub = 0 ;
        this->m_literals[nb_variables + *it - sub] = true;
    }
}

bool Clause::contains_literal(int literal) {
		int sub ;
    assert(literal != 0 && abs(literal) <= m_nb_variables);
    if(literal > 0)
    	sub = 1 ;
    else
    	sub = 0 ;
    return this->m_literals[this->m_nb_variables + literal - sub];
}

void Clause::add(int literal) {
		int sub ;
		if(!this->contains_literal(literal)) {
			this->m_size ++ ;
			if(literal > 0)
				sub = 1 ;
			else
				sub = 0 ;
			this->m_literals[this->m_nb_variables + literal - sub] = true ;
		}
}	

void Clause::remove(int literal) {
		int sub ;
		if(this->contains_literal(literal)) {
			this->m_size -- ;
			if(literal > 0)
				sub = 1 ;
			else
				sub = 0 ;
			this->m_literals[this->m_nb_variables + literal - sub] = false ;
		}
}
