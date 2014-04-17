#include "formula.h"
#include <cassert>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <climits>
#include <math.h>
#include <time.h>
#include <map>

#include "config.h"


using namespace satsolver;


void print_space() {
		std::cout << "TIME : " << (static_cast<float>(clock()))/CLOCKS_PER_SEC << "\t" ;
    for(int i = 0 ; i < ded_depth ; i++)
        std::cout << "\t" ;
}

Formula::Formula(std::vector<std::shared_ptr<Clause>> v, int nb_variables) : aff(new Affectation(nb_variables)), clauses(v), nb_variables(nb_variables), mem(), to_do(), ded(new Deductions()) {
    for(auto c : this->clauses) {
        c->set_affectation(this->aff) ;
    }
    this->ded_depth = 0 ;
    this->clean() ;
    for(auto c : this->clauses) {
        if(c->get_size() == 0)
            throw Conflict() ;
        if(WITH_WL)
            c->init_WL() ;
    }
}

satsolver::Formula::Formula(const satsolver::Formula &that) : aff(new Affectation(that.aff)), clauses(), nb_variables(that.nb_variables), mem(that.mem), to_do(that.to_do), ded(new Deductions(that.ded)) {
    this->clauses.reserve(that.clauses.size());
    std::shared_ptr<Clause> c2;
    this->depth = that.depth_ded ;
    for(auto c : that.clauses) {
        c2 = std::shared_ptr<Clause>(new Clause(*c));
        this->clauses.push_back(c2);
        c2->set_affectation(this->aff) ;
    }
    for(auto c : this->clauses) {
        assert(c->get_size() != 0);
        if(WITH_WL)
            c->init_WL() ;
    }
}

Formula::~Formula() {
    this->clauses.clear() ;
    delete this->aff ;
    delete this->ded ;
}
Formula& Formula::operator=(const Formula &that) {
    this->nb_variables = that.nb_variables;
    this->clauses = that.clauses;
    delete this->aff;
    this->aff = that.aff ;
    this->mem = that.mem ;
    this->to_do = that.to_do ;
    this->ded_depth = that.ded_depth ;
    delete this->depth ;
    this->depth = that->depth ;
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
    for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
        oss << this->clauses[i]->to_string2() << std::endl;
    }
    return oss.str() ;
}

std::set<std::set<int> > Formula::to_set() {
    std::set<std::set<int> > set;
    for(auto clause : this->clauses) {
        if(!clause->is_evaluated_to_true())
            set.insert(clause->to_set()) ;
    }
    return set ;
}

std::set<Clause*> Formula::to_clauses_set() const {
    std::set<Clause*> set;
    for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
        set.insert(new Clause(*this->clauses[i].get())) ;
    }
    return set ;
}

std::vector<std::shared_ptr<Clause>>& Formula::to_clauses_vector() {
    return this->clauses;
}

void Formula::add_clause(std::shared_ptr<satsolver::Clause> clause) {
    this->clauses.push_back(clause);
}


bool Formula::set_true(int x, unsigned int *clause_id) {
    int literal ;
    if(WITH_WL) {
        for(unsigned int i=0; i<this->clauses.size(); i++) {
            literal = this->clauses[i]->set_true(x) ;
            if(literal){// on a engendré un monome
                if(this->to_do.find(-literal) != this->to_do.end()) { // conflit
                    this->to_do.clear();
                    if(VERBOSE) {
                        print_space() ;
                        std::cout << "[Watched Literals] Detected a conflict : " << literal << std::endl ;
                    }
                    if (clause_id)
                        *clause_id = i;
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
        return deduce_true(literal, clause_id) ;
    }
    return true ;
}

bool Formula::deduce_true(int x, unsigned int *clause_id) {
    if(VERBOSE) {
        print_space() ;
        std::cout << "Deduce " << x << std::endl ;
    }
    if(this->aff->is_unknown(x)) {
        this->mem.push_back(std::pair<int,bool>(x,true)) ;
        return this->set_true(x, clause_id) ;
    }
    else
        return this->aff->is_true(x) ;
}
bool Formula::deduce_false(int x, unsigned int *clause_id) {
    return deduce_true(-x, clause_id) ;
}

bool Formula::bet_true(int x, unsigned int *clause_id) {
    if(VERBOSE) {
        print_space() ;
        std::cout << "Bet " << x << std::endl ;
    }
    ded_depth ++ ;
    if(this->aff->is_unknown(x)) {
        this->mem.push_back(std::pair<int,bool>(x,false)) ;
        return this->set_true(x, clause_id) ;
    }
    else
        return this->aff->is_true(x) ;
}
bool Formula::bet_false(int x, unsigned int *clause_id) {
    return bet_true(-x, clause_id) ;
}

int Formula::back() {
    ded_depth -- ;
    if(VERBOSE) {
        print_space() ;
        std::cout << "Backtrack : " ;
    }
    std::pair<int,bool> p ;
    while(!this->mem.empty()) {
        p = this->mem.back() ;
        if(VERBOSE) std::cout << p.first << " " ;
        this->aff->set_unknown(p.first) ;
        this->mem.pop_back() ;
        if(!p.second) {// on est arrivé au pari
            if(VERBOSE)
                std::cout << std::endl ;
            return p.first ;
        }
        this->ded->remove_deduction(p.first) ;
    }
    if(VERBOSE)
        std::cout << std::endl ;
    return 0 ;
}

int Formula::monome(unsigned int *clause_id) {
    int literal ;
    for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
        literal = this->clauses[i]->monome() ;
        if(literal) {
            if (clause_id)
                *clause_id = i;
            return literal ;
        }
    }
    return 0 ;
}

int Formula::isolated_literal(unsigned int *clause_id) {
    unsigned int i;
    int *l ;
    std::vector<std::shared_ptr<Clause>> clauses = this->to_clauses_vector();
    std::map<int, unsigned int> literal_to_clause_id;
    l = static_cast<int*>(malloc((this->nb_variables+1)*sizeof(int)));
    /* l[i] = 0 si i est inconnu et n'existe pas dans les clauses
         l[i] = 1 si i est inconnu et seulement positif
         l[i] = 2 si i est inconnu et seulement négatif
         l[i] = 3 si i est inconnu et positif et négatif
    */
    memset(l, 0, (this->nb_variables+1)*sizeof(int)) ;
    for (i=0; i<clauses.size(); i++) {
        if(clauses.at(i)->is_evaluated_to_true())
            continue;
        for(auto j : clauses.at(i)->to_set()) {
            if(j>0) {
                if(l[j] == 0 || l[j] == 1)
                    l[j] = 1 ;
                else
                    l[j] = 3 ;
            }
            else {
                if(l[-j] == 0 || l[-j] == 2)
                    l[-j] = 2 ;
                else
                    l[-j] = 3 ;
            }
            if (clause_id && l[abs(j)] != 0)
                literal_to_clause_id[j] = i;
        }
    }
    for(int i = 1 ; i <= this->nb_variables ; i++) {
        if(l[i] == 1) {
            free(l);
            if (clause_id)
                *clause_id = literal_to_clause_id[i];
            return i ;
        }
        else if(l[i] == 2) {
            free(l);
            if (clause_id)
                *clause_id = literal_to_clause_id[i];
            return -i ;
        }
    }
    free(l);
    return 0 ;
}


bool Formula::is_empty() const {
    return (this->clauses.size() == 0);
}
long unsigned int Formula::get_size() const {
    return this->clauses.size() ;
}
int Formula::get_nb_variables() const {
    return this->nb_variables;
}
bool Formula::contains_false_clause(unsigned int *clause_id) const {
    for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
        if (this->clauses[i]->is_evaluated_to_false()) {
            if (clause_id)
                *clause_id = i;
            return true;
        }
    }
    return false;
}
bool Formula::only_true_clauses(unsigned int *clause_id) const {
    for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
        if (!this->clauses[i]->is_evaluated_to_true()) {
            if (clause_id)
                *clause_id = i;
            return false;
        }
    }
    return true;
}


bool to_delete(std::shared_ptr<Clause> c, std::unordered_set<int> &literals_to_delete) {
    for(auto x : literals_to_delete) {
        if(c->contains_literal(x))
            return true ;
    }
    return false ;
}

void Formula::clean() {
    // Affectation des monomes
    std::unordered_set<int> literals_to_delete = std::unordered_set<int>() ;
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
            this->mem.push_back(std::pair<int,bool>(literal,true)) ;
            literals_to_delete.insert(literal) ;
            for(auto c : this->clauses) {
                c->remove(-literal) ;
            }
        }
    } while(literal != 0) ;
    // Suppression des clauses
 //   int n = 0 ;
    std::vector<std::shared_ptr<Clause>> old_clauses(this->clauses) ;
    this->clauses.clear() ;
    this->clauses.reserve(old_clauses.size()) ;
//    unsigned j ;
    for(unsigned i = 0 ; i < old_clauses.size() ; i++) {
/*	Les lignes suivantes suppriment les clauses contenant d'autres clauses : cela prend beaucoup trop de temps
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
*/
		if(!to_delete(old_clauses[i],literals_to_delete))
			this->clauses.push_back(old_clauses[i]) ;
    }

}


std::vector<std::pair<int,bool>> Formula::get_mem() {
    return std::vector<std::pair<int,bool>>(this->mem) ;
}

Affectation *Formula::get_aff() {
    return (this->aff) ;
}

Deductions *Formula::get_ded() {
    return (this->ded) ;
}

std::vector<int> Formula::get_unknown_literals(void) const {
	std::vector<int> v = std::vector<int>() ;
	for(unsigned i = 0 ; i < clauses.size() ; i++) {
		clauses[i]->add_literals_to_vector(v) ;
	}
	return v ;
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

int Formula::choose_literal_random() const {
	std::vector<int> v = this->get_unknown_literals() ;
	assert(v.size() > 0) ;
	return v[rand()%v.size()] ;
}

int Formula::choose_literal_moms() const {
	bool there_is_literals = false ;
	long unsigned min_clause_size = ULONG_MAX;
	std::vector<int> v = std::vector<int>() ;
	int *count = static_cast<int*>(malloc((2*this->nb_variables+1)*sizeof(int))); // count[i] = nombre d'apparitions du littéral i-nb_variables
	for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
		v.clear() ;
		this->clauses[i]->add_literals_to_vector(v) ;
		there_is_literals = there_is_literals || v.size() > 0 ;
		if(v.size() < min_clause_size && v.size() > 0) { // trouvé une clause plus petite, on remet tout à zéro
			min_clause_size = v.size() ;
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
	assert(there_is_literals) ;
	return max_literal-this->nb_variables ;
}

int Formula::choose_literal_dlis() const {
	bool there_is_literals = false ;
	std::vector<int> v = std::vector<int>() ;
	// count[i] = nombre d'apparitions du littéral i-nb_variables
	std::vector<double> count = std::vector<double>(2*this->nb_variables+1,0.0) ;
	double point ;
	for(unsigned i = 0 ; i < this->clauses.size() ; i++) {
		v.clear() ;
		this->clauses[i]->add_literals_to_vector(v) ;
		there_is_literals = there_is_literals || v.size() > 0 ;
		point = pow(2,-static_cast<double>(v.size()));
		for(unsigned j = 0 ; j < v.size() ; j ++) {
			count[v[j]+this->nb_variables] += point ;
		}
	}
	// Recherche du maximum dans count
	double max_occurence ;
	int max_literal ;
	max_occurence = 0 ;  max_literal = 0 ;
	for(int i = 0 ; i < 2*this->nb_variables+1 ; i++) {
		if(count[i] > max_occurence) {
			max_occurence = count[i] ;
			max_literal = i ;
		}
	}
	assert(there_is_literals) ;
	return max_literal-this->nb_variables ;
}

int Formula::choose_literal(int choice) {
	int literal=0 ;
	switch(choice) {
		case DUMB : literal = this->choose_literal_dumb() ; break ;
		case RANDOM : literal = this->choose_literal_random() ; break ;
		case MOMS : literal = this->choose_literal_moms() ; break ;
		case DLIS : literal = this->choose_literal_dlis() ; break ;
	}
	assert(literal && this->aff->is_unknown(literal)) ;
	return(literal) ;
}
