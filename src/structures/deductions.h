#ifndef STRUCTURES_DEDUCTIONS_H
#define STRUCTURES_DEDUCTIONS_H

#include <vector>
#include <set>
#include <unordered_set>

#include "structures/affectation.h"

class Deductions {
    private:
        // known_to_deduced[i] = ensemble des variables déduits grâce à i
        // deduced_to_known[i] = ensemble des variables ayant permi de déduire i
        // deduced_to_clause_id[i] = indice de la clause ayant permi de déduire i
        // deduction_depth[i] = profondeur de la déduction du littéral i
        // known_to_deduced[0] et deduced_to_known[0] inutilisés
        // On stock les variables et non les littéraux (valeurs absolues)
        std::vector<std::unordered_set<int>> known_to_deduced;
        std::vector<std::unordered_set<int>> deduced_to_known;
        std::vector<int> deduced_to_clause_id ;
        std::vector<int> deduction_depth ;

    public:
        // Initialisation
        Deductions(int nb_var);
        Deductions(Deductions *that);

        bool has_variable(int var) const;
        bool has_literal(int literal) const;
        std::unordered_set<int> get_deduced_from(const satsolver::Affectation &aff, int literal) const;
        std::unordered_set<int> get_deductions(const satsolver::Affectation &aff, int literal) const;
        int get_clause_id(int literal) const ;
        int get_deduction_depth(int literal) const ;
        void add_deduction(int literal, const std::unordered_set<int> &clause, int clause_id, int depth);
        void add_deduction(int literal, const std::set<int> &clause, int clause_id, int depth);
        void add_bet(int literal, int depth) ;
        void remove_deduction(int literal) ;
	
        void print() const;
        void print_edges(FILE *graph_file, const satsolver::Affectation &aff) const ;
        void print_UIP(FILE *graph_file, const satsolver::Affectation &aff, int bet, int conflict) const ;
        
        void make_conflict_graph(const satsolver::Affectation &aff, int root, int literal) const;
};

#endif
