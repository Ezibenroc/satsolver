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


void print_space(int depth) {
    std::cout << "TIME : " << (static_cast<float>(clock()))/CLOCKS_PER_SEC << "\t" ;
    for(int i = 0 ; i < depth ; i++)
        std::cout << "\t" ;
}

Formula::Formula(std::vector<std::shared_ptr<Clause>> v, int nb_variables) : Formula(v, nb_variables, NULL) {
}
Formula::Formula(std::vector<std::shared_ptr<Clause>> v, int nb_variables, std::vector<unsigned int> *affected_literals) : aff(new Affectation(nb_variables)), clauses(v), nb_variables(nb_variables), mem(), to_do(), ded(new Deductions(nb_variables)),ded_depth(0) { 
    for(auto c : this->clauses) {
        c->set_affectation(this->aff) ;
    }
    this->clean(affected_literals) ;
    for(auto c : this->clauses) {
        if(c->get_size() == 0)
            throw Conflict() ;
        if(WITH_WL)
            c->init_WL() ;
    }
}

satsolver::Formula::Formula(const satsolver::Formula &that) : aff(new Affectation(that.aff)), clauses(), nb_variables(that.nb_variables), mem(that.mem), to_do(that.to_do), ded(new Deductions(that.ded)), ded_depth(that.ded_depth)  {
    this->clauses.reserve(that.clauses.size());
    std::shared_ptr<Clause> c2;
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
    delete this->ded ;
    this->ded = that.ded ;
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

unsigned int Formula::add_clause(std::shared_ptr<satsolver::Clause> clause) {
    this->clauses.push_back(clause);
    return static_cast<unsigned int>(this->clauses.size())-1;
}


bool Formula::set_true(int x, int *clause1, int *clause2, int *literal) {
 //   std::cout << this->aff->to_string() << std::endl ;
    int l ;
    int clause_id=-1 ;
    std::unordered_set<std::pair<int,unsigned int>,Hash,Equal>::const_iterator it ;
    if(WITH_WL) {
        for(unsigned int i=0; i<this->clauses.size(); i++) {
            l = this->clauses[i]->set_true(x) ;
            if(l){// on a engendré un monome
                assert(this->get_aff()->is_unknown(l)) ;
                if((it=this->to_do.find(std::pair<int,int>(-l,0))) != this->to_do.end()) { // conflit
                    if(VERBOSE) {
                        print_space(this->ded_depth) ;
                        std::cout << "[Watched Literals] Detected a conflict : " << l << std::endl ;
                    }
                    *clause1 = it->second ;
                    *clause2 = i ;
                    *literal = l ;
                    this->to_do.clear();
                    return false ;
                }
                this->to_do.insert(std::pair<int,unsigned int>(l,i)) ;
            }
        }
    }
    this->aff->set_true(x) ;
    if(!this->to_do.empty()) { // on doit affecter ces littéraux
        l = this->to_do.begin()->first ;
        clause_id= this->to_do.begin()->second ;
        this->to_do.erase(this->to_do.begin()) ;
        return deduce_true(l, clause_id, clause1, clause2, literal) ;
    }
    return true ;
}

bool Formula::deduce_true(int x, int clause_id, int *clause1, int *clause2, int *literal) {
    if(VERBOSE) {
        print_space(this->ded_depth) ;
        std::cout << "Deduce " << x << std::endl ;
        if(clause_id >= 0) {
            print_space(this->ded_depth) ;
            std::cout << this->clauses[clause_id]->to_string() << " => " << x << std::endl;
        }
    }
    if(this->aff->is_unknown(x)) {
        if(WITH_CL || CL_INTERACT) {
            if(clause_id >= 0)
                this->ded->add_deduction(x, this->clauses[clause_id]->whole_to_set(),clause_id,this->ded_depth);
            else
                this->ded->add_bet(x,this->ded_depth) ; // pas déduit grâce à une clause, on sauvegarde simplement sa profondeur
        }
        this->mem.push_back(std::pair<int,bool>(x,true)) ;
        return this->set_true(x, clause1, clause2, literal) ;
    }
    else {
        return this->aff->is_true(x) ;
    }
}
bool Formula::deduce_false(int x, int clause_id) {
    return deduce_true(-x, clause_id, NULL, NULL, NULL) ;
}

bool Formula::bet_true(int x, int *clause1, int *clause2, int *literal) {
    if(VERBOSE) {
        print_space(this->ded_depth) ;
        std::cout << "Bet " << x << std::endl ;
    }
    this->ded_depth ++ ;
    if(this->aff->is_unknown(x)) {
        this->ded->add_bet(x,this->ded_depth) ;
        this->mem.push_back(std::pair<int,bool>(x,false)) ;
        return this->set_true(x,clause1,clause2,literal) ;
    }
    else
        return this->aff->is_true(x) ;
}
bool Formula::bet_false(int x, int *clause1, int *clause2, int *literal) {
    return bet_true(-x,clause1,clause2,literal) ;
}

int Formula::back(theorysolver::AbstractAssistant *assistant) {
    ded_depth -- ;
    if(VERBOSE) {
        print_space(this->ded_depth) ;
        std::cout << "Backtrack: " ;
    }
    std::pair<int,bool> p ;
    while(!this->mem.empty()) {
        p = this->mem.back() ;
        if(VERBOSE) std::cout << p.first << " " ;
        this->aff->set_unknown(p.first) ;
        this->mem.pop_back() ;
        this->ded->remove_deduction(p.first) ;
        assistant->on_flip(abs(p.first));
        if(!p.second) {// on est arrivé au pari
            if(VERBOSE)
                std::cout << std::endl ;
            return p.first ;
        }
    }
    if(VERBOSE)
        std::cout << std::endl ;
    return 0 ;
}

int Formula::back(theorysolver::AbstractAssistant *assistant, unsigned int depth) {
    int l=0 ;
    if(VERBOSE) {
        print_space(this->ded_depth) ;
        std::cout << "Performing " << this->ded_depth - depth << " backtrack(s)."  << std::endl ;
    }
    assert(depth <= this->ded_depth) ;
    while(depth != this->ded_depth) {
        l = this->back(assistant) ;
    }
    return l ;
}

int Formula::monome(int *clause_id) {
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

int Formula::isolated_literal(int *clause_id) {
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
bool Formula::contains_false_clause(int *clause_id) const {
    for(unsigned i = 0 ; i < this->clauses.size() ; i ++) {
        if (this->clauses[i]->is_evaluated_to_false()) {
            if (clause_id)
                *clause_id = i;
            return true;
        }
    }
    return false;
}
bool Formula::only_true_clauses(int *clause_id) const {
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

void Formula::clean(std::vector<unsigned int> *affected_literals) {
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
            if (affected_literals)
                affected_literals->push_back(abs(literal));
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
/*    Les lignes suivantes suppriment les clauses contenant d'autres clauses : cela prend beaucoup trop de temps
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

int Formula::get_ded_depth() {
    return this->ded_depth ;
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



int Formula::learn_clause(CLProof *proof, int *clause_id, unsigned int *new_depth, int literal, Clause *learned_clause) {
    long unsigned int i_conf; // The index in the memory “stack” of the literal we are making the resolution on.
    int lit_conf, lit_inf;
    int clause_id1, clause_id2 ;
    int nb_same_lvl ;
    clause_id1 = *clause_id ;
    clause_id2 = this->ded->get_clause_id(literal) ;
    std::unordered_set<int> clause = this->clauses[clause_id1]->whole_to_set() ;
    std::unordered_set<int> clause2 = this->clauses[clause_id2]->whole_to_set() ;
/*
        std::cout << "CLAUSE1 : " ;
        for(auto l : clause)
            std::cout << l << " " ;
        std::cout << std::endl ;

        std::cout << "CLAUSE2 : " ;
        for(auto l : clause2)
            std::cout << l << " " ;
        std::cout << std::endl << std::endl  ;
*/
    clause.insert(clause2.begin(), clause2.end());
    clause.erase(literal) ;
    clause.erase(-literal);
    while (true) {
        // Recherche d'un UIP (seul littéral de la clause affecté au niveau d'affectation courant)
        nb_same_lvl = 0 ;
        for(auto l : clause) {
            if(this->ded->get_deduction_depth(l) == this->get_ded_depth())
                nb_same_lvl ++ ;
            if(nb_same_lvl > 1)
                break ;
        }
        assert(nb_same_lvl) ;
        if(nb_same_lvl == 1)
            break ;
        i_conf = this->mem.size()  ;
        do { // recherche du dernier littéral affecté à faux dans la clause
          assert(i_conf);
          i_conf-- ;
          assert(this->mem[i_conf].second) ;
          lit_conf = this->mem[i_conf].first ;
        } while(clause.find(-lit_conf) == clause.end() && clause.find(lit_conf) == clause.end()) ;
        clause_id2 = this->ded->get_clause_id(lit_conf) ;
        clause2 = this->clauses[clause_id2]->whole_to_set(); // Clause correspondante
/*
        std::cout << "CLAUSE1 : " ;
        for(auto l : clause)
            std::cout << l << " " ;
        std::cout << std::endl ;
        std::cout << "Literal=" << lit_conf << std::endl ;
        std::cout << "CLAUSE2 : " ;
        for(auto l : clause2)
            std::cout << l << " " ;
        std::cout << std::endl << std::endl  ;
*/
        // Resolution
        proof->insert_top(lit_conf, std::make_pair(clause_id1, Clause(nb_variables, clause)), std::make_pair(clause_id2, Clause(nb_variables, clause2)));
        clause.insert(clause2.begin(), clause2.end());
        clause.erase(lit_conf) ;
        clause.erase(-lit_conf);
    }
    proof->conclude(Clause(this->get_nb_variables(), clause));
    if(clause.size() == 1) { // cas particulier
        if(VERBOSE) {
            print_space(this->ded_depth) ;
            std::cout << "Deduced a clause of size 1: {" << *clause.begin() << "}" << std::endl ;
        }
        *new_depth = 0 ; // on backtrack tout
        *clause_id = -1 ;
        return *clause.begin() ; // on devra ensuite déduire ce littéral
        // on n'ajoute pas cette clause, pour ne pas avoir de problèmes avec WL
        // la déduction au niveau zéro est équivalente (puisqu'elle ne sera jamais enlevée)
    }
    int depth_max = -1 ;
    lit_conf = 0 ;
    lit_inf = 0 ;
    for(auto l : clause) {
        if(this->ded->get_deduction_depth(l) != static_cast<int>(this->ded_depth)) {
            if(this->ded->get_deduction_depth(l)>depth_max) { // nouveau niveau max
                depth_max = this->ded->get_deduction_depth(l) ;
                lit_inf = l ;
            }
        }
        else
            lit_conf = l ; // seul literal du niveau courant
    }
    assert(lit_conf) ;
    assert(lit_inf) ;
    assert(depth_max != -1) ;
    if (learned_clause)
        *learned_clause = Clause(nb_variables, clause, this->aff);
    this->clauses.push_back(std::shared_ptr<Clause>(new Clause(nb_variables, clause,this->aff)));
    if(WITH_WL)
        this->clauses.back()->init_WL_CL(lit_conf,lit_inf) ; // nouveaux littéraux surveillés de la clause
    if(VERBOSE) {
        print_space(this->ded_depth) ;
        std::cout << "Learned the clause " << clauses.back()->to_string() << std::endl ;
    }
    *clause_id = static_cast<int> (this->clauses.size() - 1) ;
    *new_depth = depth_max ;
    return lit_conf ;
}

int Formula::last_bet() {
    for(long unsigned i = this->mem.size() - 1 ; i > 0 ; i--)
        if(!mem[i].second)
            return mem[i].first ;
     return mem[0].first ;
}
