#include <cassert>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <set>
#include <stack>
#include <algorithm>
#include <vector>

#define DEBUG false

#define CONFLICT_GRAPH_FILE_NAME "/tmp/conflict_graph.dot"

#include "structures/deductions.h"
#include "structures/affectation.h"
#include "config.h"

#define AS_AFF(a, l) (a.is_true(l) ? l : -l)

// Case 0 inutilisée
Deductions::Deductions(int nb_var) : known_to_deduced(nb_var+1,std::unordered_set<int>()),
                                     deduced_to_known(nb_var+1,std::unordered_set<int>()),
                                     deduced_to_clause_id(nb_var+1,-1),
                                     deduction_depth(nb_var+1,-1) {
}

Deductions::Deductions(Deductions *that) : known_to_deduced(that->known_to_deduced),deduced_to_known(that->deduced_to_known),deduced_to_clause_id(that->deduced_to_clause_id),deduction_depth(that->deduction_depth)  {
}

bool Deductions::has_literal(int literal) const  {
    return this->deduced_to_known[literal].size() != 0 ;
}
bool Deductions::has_variable(int var) const  {
    return this->has_literal(var) || this->has_literal(-var) ;
}

std::unordered_set<int> Deductions::get_deduced_from(const satsolver::Affectation &aff, int literal) const {
    std::unordered_set<int> ded ;
    assert(this->deduced_to_known.size() > 0) ;
    for(auto l : this->deduced_to_known[abs(literal)]) {
        ded.insert(-AS_AFF(aff,l)) ;
    }
    return ded ;
}

std::unordered_set<int> Deductions::get_deductions(const satsolver::Affectation &aff, int literal) const {
    std::unordered_set<int> ded ;
    assert(this->known_to_deduced.size() > 0) ;
    for(auto l : this->known_to_deduced[abs(literal)]) {
        ded.insert(-AS_AFF(aff,l)) ;
    }
    return ded ;
}

int Deductions::get_clause_id(int literal) const {
    if(!(deduced_to_clause_id[abs(literal)]>=0))
        std::cout << "Fail " << literal << std::endl ;
    assert(deduced_to_clause_id[abs(literal)]>=0) ;
    return deduced_to_clause_id[abs(literal)] ;
}

int Deductions::get_deduction_depth(int literal) const {
    assert(deduction_depth[abs(literal)]>=0) ;
    return deduction_depth[abs(literal)] ;
}

void Deductions::add_deduction(int literal, const std::unordered_set<int> &clause, int clause_id, int depth) {
    if(CL_INTERACT)
        for(auto l : clause) {
            if(abs(l)==abs(literal))
                continue ;
            this->deduced_to_known[abs(literal)].insert(abs(l)) ;
            this->known_to_deduced[abs(l)].insert(abs(literal)) ;
        }
    this->deduced_to_clause_id[abs(literal)] = clause_id ;
    this->deduction_depth[abs(literal)] = depth ;
}
void Deductions::add_deduction(int literal, const std::set<int> &clause, int clause_id, int depth) {
    std::unordered_set<int> clause2(clause.begin(), clause.end());
    this->add_deduction(literal, clause2, clause_id, depth);
}

void Deductions::add_bet(int literal, int depth) {
    assert(deduction_depth[abs(literal)] == -1) ;
    deduction_depth[abs(literal)] = depth ;
}

void Deductions::remove_deduction(int literal) {
    if(CL_INTERACT) {
        this->deduced_to_known[abs(literal)].clear() ;
        this->known_to_deduced[abs(literal)].clear() ;
        for(unsigned i = 1 ; i < deduced_to_known.size() ; i++) {
            deduced_to_known[i].erase(abs(literal)) ;
            known_to_deduced[i].erase(abs(literal)) ;
        }
    }
    this->deduced_to_clause_id[abs(literal)] = -1 ;
    this->deduction_depth[abs(literal)] = -1 ;
}


void Deductions::print() const {
    if(DEBUG) std::cout << "############################" << std::endl ;
    for(unsigned i = 1 ; i < deduced_to_known.size() ; i++) {
        if(deduced_to_known[i].size() > 0) {
            if(DEBUG) std::cout << "# " << i << " deduced with " ;
            for(auto l : deduced_to_known[i]) {
                if(DEBUG) std::cout << l << " " ;
            }
            if(DEBUG) std::cout << std::endl ;
        }
    }
    if(DEBUG) std::cout << "###" << std::endl ;
    for(unsigned i = 1 ; i < known_to_deduced.size() ; i++) {
        if(known_to_deduced[i].size() > 0) {
            if(DEBUG) std::cout << "# " << i << " implied " ;
            for(auto l : known_to_deduced[i]) {
                if(DEBUG) std::cout << l << " " ;
            }
            if(DEBUG) std::cout << std::endl ;
        }
    }
    if(DEBUG) std::cout << "############################" << std::endl ;
}

void Deductions::print_edges(FILE *graph_file, const satsolver::Affectation &aff) const {
    for(unsigned i = 1 ; i < deduced_to_known.size() ; i++) {
        for(auto l : deduced_to_known[i]) {
            fprintf(graph_file, "\t\"%d\" -> \"%d\";\n", AS_AFF(aff, l), AS_AFF(aff, i));
        }
    }
}

// Un noeud est un UIP ssi tout chemin de la racine au conflit passe par lui
void Deductions::print_UIP(FILE *graph_file, const satsolver::Affectation &aff, int bet, int conflict) const {
    std::set<int> candidates_UIP ;
    std::set<int> nodes_in_path ;
    std::set<int> nodes_in_level ;
    std::vector<int> parent = std::vector<int>(aff.get_nb_var()+1,0);
    std::stack<std::pair<int,int>> DFS ; // pile de (noeud,parent)
    int node,tmp ;
    std::pair<int,int> p ;

    DFS.push(std::pair<int,int>(abs(bet),abs(bet))) ;
    for(unsigned i = 1 ; i <= aff.get_nb_var() ; i++) {
        candidates_UIP.insert(i) ;
    }
    while(!DFS.empty()) {
        p = DFS.top() ;
        DFS.pop() ;
        node = p.first ;
        nodes_in_level.insert(node) ;
        parent[node] = p.second ;
        if(DEBUG) std::cout << "DFS(" << node << ")" << std::endl ;
        if(node == abs(conflict)) {
            nodes_in_path.clear() ;
            tmp = parent[node] ;
            if(DEBUG) std::cout << "Path : " << abs(node) << " " << tmp << " " ;
            while(tmp != abs(bet)) {
                if(candidates_UIP.find(tmp)!=candidates_UIP.end())
                    nodes_in_path.insert(tmp) ;
                tmp = parent[tmp] ;
                assert(tmp) ;
                if(DEBUG) std::cout << tmp << " " ;
            }
            if(DEBUG) std::cout << std::endl ;
            nodes_in_path.insert(abs(bet)) ;
            candidates_UIP.clear() ;
            candidates_UIP.swap(nodes_in_path) ;
        }
        else {
            if(known_to_deduced[node].size() > 0) {
                for(auto l : known_to_deduced[node]) {
                    DFS.push(std::pair<int,int>(l,node)) ;
                }
            }
        }
    }
    for(auto l : nodes_in_level)
        if(candidates_UIP.find(l) == candidates_UIP.end())
            fprintf(graph_file, "\t%d [color = \"blue\"];\n", AS_AFF(aff,l));
    for(auto l : candidates_UIP)
        fprintf(graph_file, "\t%d [color = \"yellow\"];\n", AS_AFF(aff,l));
}

void Deductions::make_conflict_graph(const satsolver::Affectation &aff, int root, int literal) const{
    FILE *graph_file = NULL;

    graph_file = fopen(CONFLICT_GRAPH_FILE_NAME, "w");
    assert(graph_file) ;

    fprintf(graph_file, "digraph G {\n");

    // Arêtes
    this->print_edges(graph_file,aff) ;

    // Noeuds
    this->print_UIP(graph_file,aff,root,literal) ;
    fprintf(graph_file, "\t%d [color = \"red\"];\n", literal);
    fprintf(graph_file, "}\n");
    fclose(graph_file);
}
