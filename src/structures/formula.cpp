#include "formula.h"
#include <vector>
#include <sstream>
#include <iostream>

using namespace satsolver;


Formula::Formula(std::vector<Clause*> v, int nb_variables) : clauses(v), nb_variables(nb_variables) {
}

Formula::Formula(const satsolver::Formula *f) {
    this->nb_variables = f->nb_variables;
	for(unsigned int i = 0 ; i < f->clauses.size() ; i++) {
        this->clauses.push_back(new Clause(*(f->clauses[i])));
    }
}

Formula::~Formula() {
	this->clauses.clear() ;
}

std::string Formula::to_string() const {
	std::ostringstream oss;
	if(this->clauses.size() == 0)
		return "EMPTY FORMULA" ;
    oss << "{";
	for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
        if (i)
            oss << ", ";
		oss << this->clauses[i]->to_string();
	}
    oss << "}";
	return oss.str() ;
}

std::set<std::set<int> > Formula::to_set() {
  std::set<std::set<int> > set;
  for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
			set.insert(this->clauses[i]->to_set()) ;
	}
	return set ;
}

void Formula::set_true(int x) {
    std::vector<Clause*> new_clauses;
    Clause *new_clause;
	for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
		if (!this->clauses[i]->contains_literal(x)) {
            new_clause = new Clause(*this->clauses[i]);
			new_clause->remove(-x);
            new_clauses.push_back(new_clause);
        }
	}
    this->clauses.swap(new_clauses);
}


void Formula::set_false(int x) {
	this->set_true(-x) ;
}


int Formula::find_monome() const {
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


bool Formula::is_empty() const {
    return (this->clauses.size() == 0);
}
int Formula::get_nb_variables() const {
    return this->nb_variables;
}
bool Formula::contains_empty_clause() const {
	for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
        if (this->clauses[i]->get_size() == 0)
            return true;
    }
    return false;
}
int Formula::choose_literal() const {
	for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
        if (this->clauses[i]->get_size() != 0)
            return *(this->clauses[i]->to_set().begin());
    }
    return 0;
}
