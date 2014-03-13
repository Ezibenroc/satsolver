#include "formula.h"
#include <cassert>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>

#include "config.h"


using namespace satsolver;

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
    if (WITH_WL) {
        for(auto c : this->clauses) {
            if(c->get_size() == 0)
                throw Conflict() ;
            c->init_WL() ;
        }
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


bool Formula::set_true(int x) {
    int literal ;
    if(WITH_WL) {
        for(auto c : this->clauses) {
            literal = c->set_true(x) ;
            if(literal){// on a engendré un monome
                if(this->to_do.find(-literal) != this->to_do.end()) { // conflit
                    this->to_do.clear();
                    if(VERBOSE) {print_space() ; std::cout << "[Watched Literals] Detected a conflict : " << literal << std::endl ;}
                    return false ;
                }
                this->to_do.insert(literal) ;
            }
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
bool Formula::deduce_false(int x) {
    return deduce_true(-x) ;
}

bool Formula::bet_true(int x) {
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
bool Formula::bet_false(int x) {
    return bet_true(-x) ;
}

int Formula::back() {
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
        if(!p.second) {// on est arrivé au pari
            if(VERBOSE)
                std::cout << std::endl ;
            return p.first ;
        }
    }
    if(VERBOSE) std::cout << std::endl ;
    return 0 ;
}

int Formula::monome() {
    int literal ;
    for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
        literal = this->clauses[i]->monome() ;
        if(literal)
            return literal ;
    }
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
        if(l[i] == 1) {
            free(l);
            return i ;
        }
        else if(l[i] == 2) {
            free(l);
            return -i ;
        }
    }
    free(l);
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
int Formula::choose_literal_dumb() const {
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
            literal = c->monome_begin();
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
}


std::stack<std::pair<int,bool>> Formula::get_mem() {
    return std::stack<std::pair<int,bool>>(this->mem) ;
}

Affectation *Formula::get_aff() {
    return (this->aff) ;
}

std::vector<int> Formula::to_vector (void) const {
	std::vector<int> v = std::vector<int>() ;
	for(unsigned i = 0 ; i < clauses.size() ; i++) {
		clauses[i]->add_literals_to_vector(v) ;
	}
	return v ;
}

int Formula::choose_literal_random() const {
	std::vector<int> v = this->to_vector() ;
	assert(v.size() > 0) ;
	return v[rand()%v.size()] ;
}

int Formula::choose_literal_moms() const {
	unsigned min_clause_size = 999999999 ;
	std::vector<int> v = std::vector<int>() ;
	int *count = (int*) malloc((2*this->nb_variables+1)*sizeof(int)) ; // count[i] = nombre d'apparitions du littéral i-nb_variables
	for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
		v.clear() ;
		this->clauses[i]->add_literals_to_vector(v) ;
		if(v.size() < min_clause_size) { // trouvé une clause plus petite, on remet tout à zéro
			min_clause_size = (unsigned) v.size() ;
			memset(count,0,(2*this->nb_variables+1)*sizeof(int)) ;
		}
		if(v.size() == min_clause_size) { // on incrémente le compteur de tous les littéraux de v
			for(unsigned j = 0 ; j < v.size() ; j++) {
				count[v[j]+this->nb_variables] ++ ;
			}
		}
	}
	// Recherche du maximum dans count
	int max_occurence, max_literal ;
	max_occurence = 0 ;  max_literal = 0 ;
	for(int i = 0 ; i < 2*this->nb_variables+1 ; i++) { 
		if(count[i] > max_occurence) {
			max_occurence = count[i] ;
			max_literal = i ;
		}
	}
	free(count) ;
	return max_literal-this->nb_variables ;
}

int Formula::choose_literal_dlis() const {
	std::vector<int> v = std::vector<int>() ;
	float *count = (float*) malloc((2*this->nb_variables+1)*sizeof(float)) ; // count[i] = nombre d'apparitions du littéral i-nb_variables
	float point ;
	memset(count,0,(2*this->nb_variables+1)*sizeof(float)) ;
	for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
		v.clear() ;
		this->clauses[i]->add_literals_to_vector(v) ;
		point = (float) pow(2,-(float)v.size()) ;
		for(unsigned j = 0 ; j < v.size() ; j ++)
			count[v[j]+this->nb_variables] += point ;
	}
	// Recherche du maximum dans count
	float max_occurence ;
	int max_literal ;
	max_occurence = 0 ;  max_literal = 0 ;
	for(int i = 0 ; i < 2*this->nb_variables+1 ; i++) {
		if(count[i] > max_occurence) {
			max_occurence = count[i] ;
			max_literal = i ;
		}
	}
	free(count) ;
	return max_literal-this->nb_variables ;
}

int Formula::choose_literal(int choice) {
	switch(choice) {
		case DUMB : return this->choose_literal_dumb() ;
		case RANDOM : return this->choose_literal_random() ;
		case MOMS : return this->choose_literal_moms() ;
		case DLIS : return this->choose_literal_dlis() ;
		default : assert(false) ; return 0 ;
	}
}
