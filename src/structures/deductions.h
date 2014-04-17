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
        // known_to_deduced[0] et deduced_to_known[0] inutilisés
        // On stock les variables et non les littéraux (valeurs absolues)
        std::vector<std::unordered_set<int>> known_to_deduced;
        std::vector<std::unordered_set<int>> deduced_to_known;

    public:
        // Initialisation
        Deductions(int nb_var);
        Deductions(Deductions *that);

        bool has_variable(int var) const;
        bool has_literal(int literal) const;
        std::unordered_set<int> get_deduced_from(int literal) const;
        std::unordered_set<int> get_deductions(int literal) const;
        void add_deduction(int literal, const std::unordered_set<int> &clause);
        void add_deduction(int literal, const std::set<int> &clause);
        void remove_deduction(int literal) ;
	
        void print() const;
        void print_edges(FILE *graph_file, const satsolver::Affectation &aff) const ;
        void print_UIP(FILE *graph_file, const satsolver::Affectation &aff, int bet, int conflict) const ;
};

#endif
