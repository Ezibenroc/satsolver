#include "formula.h"
#include <cassert>
#include <vector>
#include <sstream>
#include <iostream>

using namespace satsolver;


Formula::Formula(std::vector<std::shared_ptr<Clause>> v, int nb_variables) : clauses(v), nb_variables(nb_variables) {
	this->aff = new Affectation(nb_variables) ;
	this->mem = std::stack<std::pair<int,bool>>() ;
	this->to_do = std::set<int>() ;
	for(auto c : this->clauses) {
		c->set_affectation(this->aff) ;
	}
	this->clean() ;
	for(auto c : this->clauses) {
		c->init_WL() ;
	}
}

Formula::Formula(satsolver::Formula *f) {
    this->nb_variables = f->nb_variables;
    this->clauses.reserve(f->clauses.size()) ;
    for(unsigned int i = 0 ; i < f->clauses.size() ; i++) {
        this->clauses.push_back(std::shared_ptr<Clause>(new Clause(*f->clauses[i].get())));
    }
    this->aff = new Affectation(f->aff) ;
    for(auto c : this->clauses) 
    	c->set_affectation(aff) ;
		this->mem = std::stack<std::pair<int,bool>>(f->mem) ;
		this->to_do = std::set<int>(f->to_do) ;
}

Formula::~Formula() {
    this->clauses.clear() ;
    delete this->aff ;
}
Formula& Formula::operator=(const Formula &that) {
    this->nb_variables = that.nb_variables;
    this->clauses = that.clauses;
    this->aff = that.aff ;
    this->mem = that.mem ;
    this->to_do = that.to_do ;
    return *this;
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
    	if(!this->clauses[i]->is_true())
        set.insert(this->clauses[i]->to_set()) ;
    }
    return set ;
}

std::set<Clause*> Formula::to_clauses_set() {
    std::set<Clause*> set;
    for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
        set.insert(new Clause(*this->clauses[i].get())) ;
    }
    return set ;
}

/* Beaucoup de copies... Et une fuite mémoire astronomique.
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
*/

void Formula::set_true(int x) {
	int literal ;
	this->aff->set_true(x) ;
	for(auto c : this->clauses) {
		literal = c->set_true(x) ;
		if(literal){// on a engendré un monome
			if(this->to_do.find(-literal) != this->to_do.end()) { // conflit
				throw Conflict() ;
			}
			this->to_do.insert(literal) ;
		} 
	}
	if(!this->to_do.empty()) { // on doit affecter ces littéraux
		literal = *this->to_do.begin() ;	
		this->to_do.erase(literal) ;
		deduce_true(literal) ;
	}
}

void Formula::deduce_true(int x) {
	this->mem.push(std::pair<int,bool>(x,true)) ;
	this->set_true(x) ;	
}
void Formula::deduce_false(int x) {
	deduce_true(-x) ;
}

void Formula::bet_true(int x) {
	this->mem.push(std::pair<int,bool>(x,false)) ;
	this->set_true(x) ;
}
void Formula::bet_false(int x) {
	bet_true(-x) ;
}

int Formula::back() {
	std::pair<int,bool> p ;
	while(!this->mem.empty()) {
		p = this->mem.top() ;
		this->aff->set_unknown(p.first) ;
		this->mem.pop() ;
		if(!p.second) // on est arrivé au paris
			return p.first ;
	}
	return 0 ;
}

int Formula::isolated_literal() {

}


bool Formula::is_empty() const {
    return (this->clauses.size() == 0);
}
int Formula::get_size() const {
    return (int) this->clauses.size() ;
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
    std::cout << "Error in choose_literal : all clauses are empty, or there is no clauses." << std::endl ;
    return 0;
}

		
bool to_delete(std::shared_ptr<Clause> c, std::set<int> &literals_to_delete) {
	for(auto x : literals_to_delete) {
		if(c->contains_literal(x))
			return true ;
	}
	return false ;
}

void Formula::clean() {
		// Affectation des monomes
		std::set<int> literals_to_delete = std::set<int>() ;
		int literal ;
		do {
			for(auto c : this->clauses) {
				literal = c->monome();
				if(literals_to_delete.find(literal) != literals_to_delete.end())
					literal = 0 ;
				if(literal)
					break ;
			}
			if(literal) {
				this->aff->set_true(literal) ;
				this->mem.push(std::pair<int,bool>(literal,true)) ;
				literals_to_delete.insert(literal) ;
				for(auto c : this->clauses) {
					c->remove(-literal) ;
				}
			}
		} while(literal != 0) ;
		
		// Suppression des clauses contenant d'autres clauses ou des littéraux vrais
    int n = 0 ;
    std::vector<std::shared_ptr<Clause>> old_clauses(this->clauses) ;
    this->clauses.clear() ;
    this->clauses.reserve(old_clauses.size()) ;
    unsigned j ;
    for(unsigned i = 0 ; i < old_clauses.size() ; i++) {
        j = 0 ;
        if(!to_delete(old_clauses[i],literals_to_delete)) {
		      while(j < old_clauses.size() && (old_clauses[j] == NULL || j == i
		                                     || !old_clauses[i]->contains_clause(*old_clauses[j]))) {
		          j++ ;
		    	}
        }
        if(j < old_clauses.size()) { // la clause i doit être supprimée 
            old_clauses[i] = NULL ;
            n ++ ;
        }
        else {    // la clause i ne contient aucune autre clauses
            this->clauses.push_back(old_clauses[i]) ;
        }
    }
//    std::cout << "Deleted " << n << " clauses during clean." << std::endl ;
}


std::stack<std::pair<int,bool>> Formula::get_mem() {
	return std::stack<std::pair<int,bool>>(this->mem) ;
}

Affectation *Formula::get_aff() {
	return this->aff ;
}		
