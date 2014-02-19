#include "formula.h"

using namespace satsolver;


Formula::Formula(std::vector<Clause*> v, int nb_variables) : clauses(v), aff(Affectation(nb_variables)), nb_variables(nb_variables) {
}

Formula::~Formula() {
	for(unsigned i = 0 ; i < this->clauses.size() ; i++)
		delete this->clauses[i] ;
}

std::set<std::set<int>> Formula::to_set() {
  std::set<int> tmp;
  std::set<std::set<int>> set;
  for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
  	tmp.clear() ;
  	if(!this->clauses[i]->is_true()) {
			for(int j = 1 ; j <= this->nb_variables ; j++) {
				if(this->clauses[i]->contains_literal(-j) && this->aff.is_unknown(-j))
				        tmp.insert(-j);
				if(this->clauses[i]->contains_literal(j) && this->aff.is_unknown(j))
				        tmp.insert(j);
				}
			set.insert(tmp) ;
		}
	}
	return set ;
}

int Formula::find_monome() {
	int literal ;
	for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
		literal = this->clauses[i]->monome(this->aff) ;
		if(literal)
			return literal ;
	}
	return 0 ;
}

bool Formula::unit_propagation() {
	int literal = find_monome() ;
	if(literal == 0)
		return false ;
	this->aff.set_true(literal) ;
	for(unsigned i = 0 ; i < this->clauses.size() ; i++)
		this->clauses[i]->set_true(literal) ;
	return true ;
}

