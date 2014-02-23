#include "formula.h"
#include <cassert>
#include <vector>
#include <sstream>
#include <iostream>

using namespace satsolver;


Formula::Formula(std::vector<std::shared_ptr<Clause>> v, int nb_variables) : clauses(v), nb_variables(nb_variables) {
}

Formula::Formula(satsolver::Formula *f) {
    this->nb_variables = f->nb_variables;
    this->clauses.reserve(f->clauses.size()) ;
    for(unsigned int i = 0 ; i < f->clauses.size() ; i++) {
        this->clauses.push_back(std::shared_ptr<Clause>(new Clause(*f->clauses[i].get())));
    }
}

Formula::~Formula() {
    this->clauses.clear() ;
}
Formula& Formula::operator=(const Formula &that) {
    this->nb_variables = that.nb_variables;
    this->clauses = that.clauses;
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
    std::vector<std::shared_ptr<Clause>> old_clauses(this->clauses) ;
    this->clauses.clear() ;
    this->clauses.reserve(old_clauses.size()) ;
    for(unsigned j = 0 ; j < old_clauses.size() ; j++) {
        if (!old_clauses[j]->contains_literal(x)) {
            this->clauses.push_back(old_clauses[j]) ;
            this->clauses[this->clauses.size()-1]->remove(-x) ;
        }
    }
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

/*
int Formula::unit_propagation() {
    int literal = find_monome() ;
    if(literal == 0)
        return 0 ;
    this->set_true(literal) ;
    return literal ;
}*/

int Formula::find_isolated_literal() const {
    bool pos, neg ;
    for(int variable = 1 ; variable <= this->nb_variables ; variable ++) {
        pos = false ; neg = false ;
        for(unsigned i = 0 ; !(pos && neg) && i < this->clauses.size() ; i++){
            pos = pos || this->clauses[i]->contains_literal(variable) ;
            neg = neg || this->clauses[i]->contains_literal(-variable) ;
        }
        if(pos && !neg)
            return variable ;
        else if(neg && !pos)
            return -variable ;
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

void Formula::clean() {
    int n = 0 ;
    std::vector<std::shared_ptr<Clause>> old_clauses(this->clauses) ;
    this->clauses.clear() ;
    this->clauses.reserve(old_clauses.size()) ;
    unsigned j ;
    for(unsigned i = 0 ; i < old_clauses.size() ; i++) {
        j = 0 ;
        while(j < old_clauses.size() && (old_clauses[j] == NULL || j == i
                                                        || !old_clauses[i]->contains_clause(*old_clauses[j]))) {
            j++ ;
        }
        if(j < old_clauses.size()) { // la clause i contient la clause j
            old_clauses[i] = NULL ;
            n ++ ;
        }
        else {    // la clause i ne contient aucune autre clauses
            this->clauses.push_back(old_clauses[i]) ;
        }
    }
//    std::cout << "Deleted " << n << " clauses during clean." << std::endl ;
}
