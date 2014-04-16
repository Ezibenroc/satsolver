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

        bool has_variable(int var) const;
        bool has_literal(int literal) const;
        std::unordered_set<int> get_deduced_from(int literal) const;
        std::unordered_set<int> get_deductions(int literal) const;
        void add_deduction(int literal, std::unordered_set<int> &clause);
        void add_deduction(int literal, std::set<int> clause);
        void remove_unknown(satsolver::Affectation &aff);
	
				void print() ;
};
