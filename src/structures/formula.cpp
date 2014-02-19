#include "formula.h"
#include <vector>

using namespace satsolver;


Formula::Formula(std::vector<Clause*> v, int nb_variables) : clauses(v), nb_variables(nb_variables) {
}

Formula::Formula(const satsolver::Formula &f) {
	
}

Formula::~Formula() {
	for(unsigned i = 0 ; i < this->clauses.size() ; i++)
		delete this->clauses[i] ;
}

std::set<std::set<int> > Formula::to_set() {
  std::set<std::set<int> > set;
  for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
			set.insert(this->clauses[i]->to_set()) ;
	}
	return set ;
}

void Formula::set_true(int x) {
	for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
		if(this->clauses[i]->contains_literal(x)) {
			this->clauses[i] = this->clauses[this->clauses.size()-1] ;
			this->clauses.pop_back() ;
			i-- ;
		}
		else
			this->clauses[i]->remove(-x) ;
	}
}


void Formula::set_false(int x) {
	this->set_true(-x) ;
}


int Formula::find_monome() {
	int literal ;
	for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
		literal = this->clauses[i]->monome() ;
		if(literal)
			return literal ;
	}
	return 0 ;
}

bool Formula::unit_propagation() {
	int literal = find_monome() ;
	if(literal == 0)
		return false ;
	this->set_true(literal) ;
	return true ;
}

