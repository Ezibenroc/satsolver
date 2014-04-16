#include <cassert>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <set>
#include <stack>
#include <algorithm>
#include <vector>

#include "structures/deductions.h"

#define AS_AFF(a, l) (a.is_true(l) ? l : -l)

Deductions::Deductions() : known_to_deduced(), deduced_to_known() {
}

bool Deductions::has_literal(int literal) const  {
    return this->deduced_to_known.find(literal) != deduced_to_known.end();
}
bool Deductions::has_variable(int var) const  {
    return this->deduced_to_known.find(var) != deduced_to_known.end() || deduced_to_known.find(-var) != deduced_to_known.end();
}

std::unordered_set<int> Deductions::get_deduced_from(int literal) const {
    return this->deduced_to_known.at(literal);
}

std::unordered_set<int> Deductions::get_deductions(int literal) const {
    return this->known_to_deduced.at(literal);
}

void Deductions::add_deduction(int literal, const std::unordered_set<int> &clause) {
    this->deduced_to_known[literal] = clause;
    for (auto it : this->known_to_deduced) {
        auto pos = it.second.find(literal);
        if (pos != it.second.end())
            it.second.erase(pos);
        else {
            pos = it.second.find(-literal);
            if (pos != it.second.end())
                it.second.erase(pos);
        }
    }
    for (auto it : clause) {
        auto pos = this->known_to_deduced.find(it);
        if (pos != this->known_to_deduced.end())
            this->known_to_deduced.at(it).insert(literal);
        else
            this->known_to_deduced.insert(std::make_pair(it, std::unordered_set<int>({literal})));
    }
}
void Deductions::add_deduction(int literal, const std::set<int> &clause) {
    std::unordered_set<int> clause2(clause.begin(), clause.end());
    this->add_deduction(literal, clause2);
}

void Deductions::remove_unknown(satsolver::Affectation &aff) {
    std::unordered_set<int> to_remove;
    for (auto pos=this->known_to_deduced.begin(); pos != this->known_to_deduced.end(); pos++) {
        if (aff.is_unknown(pos->first)) {
            to_remove.insert(pos->first);
        }
    }
    for (auto it : to_remove) {
        this->known_to_deduced.erase(it);
        for (auto it2 : this->deduced_to_known) {
            it2.second.erase(it);
        }
    }
}


void Deductions::print() const {
    for(auto it : this->deduced_to_known) {
        std::cout << it.first << " deduced with " ;
        for(auto l : it.second) {
            std::cout << l << " " ;
        }
        std::cout << std::endl ;
    }
}

void Deductions::print_edges(FILE *graph_file, const satsolver::Affectation &aff) const {
    for(auto it : this->known_to_deduced) {
        for(auto l : it.second) {
            if(abs(l)!=abs(it.first))
                fprintf(graph_file, "\t\"%d\" -> \"%d\";\n", AS_AFF(aff, it.first), AS_AFF(aff, l));
        }
    }
}

// Un noeud est un UIP ssi tout chemin de la racine au conflit passe par lui
void Deductions::print_UIP(FILE *graph_file, const satsolver::Affectation &aff, int bet, int conflict) const {
    std::set<int> candidates_UIP ;
    std::set<int> nodes_in_path ;
    std::vector<int> parent = std::vector<int>(aff.get_nb_var()+1,0);
    std::stack<int> DFS ;
    int node,tmp ;
    
    DFS.push(bet) ;
    parent[abs(bet)] = abs(bet) ;
    for(unsigned i = 1 ; i <= aff.get_nb_var() ; i++) {
        candidates_UIP.insert(i) ;
        candidates_UIP.insert(-i) ;
    }
    while(!DFS.empty()) {
        node = DFS.top() ;
        DFS.pop() ;
        if(node == conflict) {
            nodes_in_path.clear() ;
            tmp = parent[abs(node)] ;
            while(tmp != abs(bet)) {
                if(candidates_UIP.find(tmp)!=candidates_UIP.end())
                    nodes_in_path.insert(AS_AFF(aff,tmp)) ;
                tmp = parent[tmp] ;
                assert(tmp) ;
            }
            nodes_in_path.insert(bet) ;
            candidates_UIP.clear() ;
            candidates_UIP.swap(nodes_in_path) ;
        }   
        else {
            node = -node ;
            if(this->known_to_deduced.find(node)!=this->known_to_deduced.end()) {
                for(auto l : get_deductions(node)) {
                    if(l!=node) {
                        DFS.push(l) ;
                        parent[abs(l)] = abs(node) ;
                    }   
                }
            }
        }
    }
    for(auto l : candidates_UIP)
        fprintf(graph_file, "\t%d [color = \"yellow\"];\n", AS_AFF(aff,l));
}
