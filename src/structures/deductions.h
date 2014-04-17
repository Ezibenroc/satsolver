#ifndef STRUCTURES_DEDUCTIONS_H
#define STRUCTURES_DEDUCTIONS_H

#include <map>
#include <set>
#include <unordered_set>

#include "structures/affectation.h"

class Deductions {
    private:
        std::map<int, std::unordered_set<int>> known_to_deduced;
        std::map<int, std::unordered_set<int>> deduced_to_known;

    public:
        Deductions();
        Deductions(Deductions *that);

        bool has_variable(int var) const;
        bool has_literal(int literal) const;
        std::unordered_set<int> get_deduced_from(int literal) const;
        std::unordered_set<int> get_deductions(int literal) const;
        void add_deduction(int literal, const std::unordered_set<int> &clause);
        void add_deduction(int literal, const std::set<int> &clause);
        void remove_deduction(int literal) ;
        void remove_unknown(satsolver::Affectation &aff);
	
        void print() const;
        void print_edges(FILE *graph_file, const satsolver::Affectation &aff) const ;
        void print_UIP(FILE *graph_file, const satsolver::Affectation &aff, int bet, int conflict) const ;
};

#endif
