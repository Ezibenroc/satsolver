#include "formula.h"
#include <cassert>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>

#include "config.h"

using namespace satsolver;

bool verbose = false ;
int depth_stack = 0 ; // used only for verbosity mode

void print_space() {
	for(int i = 0 ; i < depth_stack ; i++)
		std::cout << "\t" ;
}

Formula::Formula(std::vector<std::shared_ptr<Clause>> v, int nb_variables) : clauses(v), nb_variables(nb_variables) {
	this->aff = new Affectation(nb_variables) ;
	this->mem = std::stack<std::pair<int,bool>>() ;
	this->to_do = std::set<int>() ;
	for(auto c : this->clauses) {
		c->set_affectation(this->aff) ;
	}
	this->clean() ;
    if (WITH_WL)
        for(auto c : this->clauses)
            c->init_WL() ;
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

std::string Formula::to_string() {
    std::ostringstream oss;
    bool b  ;
    std::set<std::set<int>> s = this->to_set() ;
    if(s.empty())
    	return "EMPTY FORMULA" ;
    for(auto c : s) {
    	oss << "{" ;
    	b = false ;
    	for(auto i : c) {
    		if(b)
    			oss << "," ;
    		oss << i ;
    		b = true ;
    	}
    	oss << "}" ;
    }
    return oss.str() ;
}

std::string Formula::to_string2() {
    std::ostringstream oss;
    oss << "######\n" ;
    for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
    	oss << "# " << this->clauses[i]->to_string() << "\n";
    } 
    oss << "######\n" ;
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

bool Formula::set_true(int x) {
	int literal ;
/*	std::cout << "------------------------" << std::endl ;
	std::cout << "Stack size : " << this->mem.size() << std::endl ;
	std::cout << this->aff->to_string() << std::endl ;
	std::cout << this->to_string2() << std::endl ;
	std::cout << "------------------------" << std::endl ;*/
	for(auto c : this->clauses) {
		literal = c->set_true(x) ;
		if(WITH_WL && literal){// on a engendré un monome
			if(this->to_do.find(-literal) != this->to_do.end()) { // conflit
				this->to_do.clear();
				if(verbose) std::cout << "Generated a conflict : " << literal << std::endl ;
				return false ;
			}
			this->to_do.insert(literal) ;
		} 
	}
	this->aff->set_true(x) ;
	if(!this->to_do.empty()) { // on doit affecter ces littéraux
		literal = *this->to_do.begin() ;	
		this->to_do.erase(literal) ;
		return deduce_true(literal) ;
	}
	return true ;
}

bool Formula::deduce_true(int x) {
	if(verbose) {
		print_space() ;
		std::cout << "Deduce " << x << std::endl ;
	}
	if(this->aff->is_unknown(x)) {
		this->mem.push(std::pair<int,bool>(x,true)) ;
		return this->set_true(x) ;	
	}
	else return this->aff->is_true(x) ;	
}
bool Formula::deduce_false(int x) {
	return deduce_true(-x) ;
}

bool Formula::bet_true(int x) {
	if(verbose) {
		print_space() ;
		std::cout << "Bet " << x << std::endl ;
	}
	depth_stack ++ ;
	if(this->aff->is_unknown(x)) {
		this->mem.push(std::pair<int,bool>(x,false)) ;
		return this->set_true(x) ;
	}
	else return this->aff->is_true(x) ;
}
bool Formula::bet_false(int x) {
	return bet_true(-x) ;
}

int Formula::back() {
	depth_stack -- ;
	if(verbose) {
		print_space() ;
		std::cout << "Backtrack : " ;
	}
	std::pair<int,bool> p ;
	while(!this->mem.empty()) {
		p = this->mem.top() ;
		if(verbose) std::cout << p.first << " " ;
		this->aff->set_unknown(p.first) ;
		this->mem.pop() ;
		if(!p.second) {// on est arrivé au paris
			if(verbose)
				std::cout << std::endl ;
			return p.first ;
		}
	}
	if(verbose) std::cout << std::endl ;
	return 0 ;
}

int Formula::isolated_literal() {
	int *l ;
	l = (int*) malloc((this->nb_variables+1)*sizeof(int)) ;
	/* l[i] = 0 si i est inconnu et n'existe pas dans les clauses
		 l[i] = 1 si i est inconnu et seulement positif
		 l[i] = 2 si i est inconnu et seulement négatif
		 l[i] = 3 si i est inconnu et positif et négatif
	*/
	memset(l,0,(this->nb_variables+1)*sizeof(int)) ;
	for(auto c : this->to_set()) {
		for(auto i : c) {
			if(i>0) {
				if(l[i] == 0 || l[i] == 1)
					l[i] = 1 ;
				else
					l[i] = 3 ;
			}
			else {
				if(l[-i] == 0 || l[-i] == 2)
					l[-i] = 2 ;
				else
					l[-i] = 3 ;
			}
		}
	}
	for(int i = 1 ; i <= this->nb_variables ; i++) {
		if(l[i] == 1)
			return i ;
		else if(l[i] == 2)
			return -i ;
	}
	return 0 ;
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
bool Formula::contains_false_clause() const {
    for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
        if (this->clauses[i]->is_evaluated_to_false())
            return true;
    }
    return false;
}
int Formula::choose_literal() const {
    for(int i = 1 ; i <= this->nb_variables ; i ++) {
        if (this->aff->is_unknown(i)) {
        	return i ;
        }
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
	return new Affectation(this->aff) ;
}		
