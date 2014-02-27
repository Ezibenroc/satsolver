#include <cstring>
#include <iostream>
#include "config.h"
#include "formula_with_WL.h"

using namespace satsolver;

int depth_stack = 0 ; // used only for verbosity mode
void print_space() {
	for(int i = 0 ; i < depth_stack ; i++)
		std::cout << "\t" ;
}

FormulaWithWL::FormulaWithWL(satsolver::Formula *f) : Formula(f) {
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

FormulaWithWL::FormulaWithWL(satsolver::FormulaWithWL *f) : Formula(f) {
    this->aff = new Affectation(f->aff) ;
    for(auto c : this->clauses) 
        c->set_affectation(aff) ;
    this->mem = std::stack<std::pair<int,bool>>(f->mem) ;
    this->to_do = std::set<int>(f->to_do) ;
}

FormulaWithWL::~FormulaWithWL() {
    this->clauses.clear() ;
    delete this->aff ;
}
FormulaWithWL& FormulaWithWL::operator=(const FormulaWithWL &that) {
    this->nb_variables = that.nb_variables;
    this->clauses = that.clauses;
    this->aff = that.aff ;
    this->mem = that.mem ;
    this->to_do = that.to_do ;
    return *this;
}

bool FormulaWithWL::set_true(int x) {
	int literal ;
/*	std::cout << "------------------------" << std::endl ;
	std::cout << "Stack size : " << this->mem.size() << std::endl ;
	std::cout << this->aff->to_string() << std::endl ;
	std::cout << this->to_string2() << std::endl ;
	std::cout << "------------------------" << std::endl ;*/
    for(auto c : this->clauses) {
        literal = c->set_true(x) ;
        if(literal){// on a engendré un monome
            if(this->to_do.find(-literal) != this->to_do.end()) { // conflit
                this->to_do.clear();
                if(VERBOSE) std::cout << "Generated a conflict : " << literal << std::endl ;
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

bool FormulaWithWL::deduce_true(int x) {
	if(VERBOSE) {
		print_space() ;
		std::cout << "Deduce " << x << std::endl ;
	}
	if(this->aff->is_unknown(x)) {
		this->mem.push(std::pair<int,bool>(x,true)) ;
		return this->set_true(x) ;	
	}
	else return this->aff->is_true(x) ;	
}
bool FormulaWithWL::deduce_false(int x) {
	return deduce_true(-x) ;
}

bool FormulaWithWL::bet_true(int x) {
	if(VERBOSE) {
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
bool FormulaWithWL::bet_false(int x) {
	return bet_true(-x) ;
}

int FormulaWithWL::back() {
	depth_stack -- ;
	if(VERBOSE) {
		print_space() ;
		std::cout << "Backtrack : " ;
	}
	std::pair<int,bool> p ;
	while(!this->mem.empty()) {
		p = this->mem.top() ;
		if(VERBOSE) std::cout << p.first << " " ;
		this->aff->set_unknown(p.first) ;
		this->mem.pop() ;
		if(!p.second) {// on est arrivé au paris
			if(VERBOSE)
				std::cout << std::endl ;
			return p.first ;
		}
	}
	if(VERBOSE) std::cout << std::endl ;
	return 0 ;
}

int FormulaWithWL::isolated_literal() {
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

int FormulaWithWL::choose_literal() const {
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

void FormulaWithWL::clean() {
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


std::stack<std::pair<int,bool>> FormulaWithWL::get_mem() {
	return std::stack<std::pair<int,bool>>(this->mem) ;
}

Affectation *FormulaWithWL::get_aff() {
	return new Affectation(this->aff) ;
}		

std::set<std::set<int> > FormulaWithWL::to_set() const {
    std::set<std::set<int> > set;
    for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
    	if(!this->clauses[i]->is_true())
            set.insert(this->clauses[i]->to_set()) ;
    }
    return set ;
}
