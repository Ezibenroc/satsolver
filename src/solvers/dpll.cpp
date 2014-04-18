#include <set>
#include <map>
#include <iostream>
#include <cassert>
#include <memory>

#include <exception>
#include <stdexcept>

#include "structures/affectation.h"
#include "structures/deductions.h"
#include "structures/CL_proof.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "solvers/dpll.h"
#include "config.h"

#define CL_PROOF_FILE_NAME "/tmp/CL_proof.tex"

using namespace satsolver;

#define AS_AFF(a, l) (a.is_true(l) ? l : -l)

unsigned int cl_interact(const Deductions &deductions, const Affectation &aff, int last_bet, int literal, bool *with_proof) {
    char mode;
    unsigned int steps;
    assert(deductions.has_variable(abs(literal)));
    std::cout << "Conflict on literal " << literal << ". Action? [g/r/c/s/t] ";
    assert(with_proof);
    *with_proof = false;
    while (true) {
        std::cin >> mode;
        switch (mode) {
            case 'g':
                deductions.make_conflict_graph(aff, last_bet, literal);
                return 1;
            case 'r':
                if (!WITH_CL) {
                    std::cout << "Clause learning is not enabled. Use -CL. [g/r/c/s/t]. " << std::endl;
                    continue;
                }
                *with_proof = true;
                return 0;
            case 'c':
                return 1;
            case 's':
                std::cin >> steps;
                return steps;
            case 't':
                CL_INTERACT = false;
                return 0;
            default:
                std::cout << "Invalid character [g/r/c/s/t]. ";
                continue;
        }
    }
}

std::shared_ptr<Clause> learn_clause(const Deductions &deductions, const std::vector<std::pair<int, bool>> &mem, const Affectation &aff, int nb_variables, int literal, CLProof *proof) {
    (void) aff;
    long unsigned int mem_top = mem.size()-1; // The index in the memory “stack” of the literal we are making the resolution on.
    std::unordered_set<int> clause(deductions.get_deduced_from(aff,literal)), clause2;
    mem_top--;
    while (mem.at(mem_top).second) {
        literal = mem.at(mem_top).first;
        assert(mem.at(mem_top).first == literal);
        try {
            clause2 = deductions.get_deduced_from(aff,literal); // This is the clause we will make the resolution with
        }
        catch (std::out_of_range) {
            break;
        }

        if (clause.find(-literal) == clause.end())
            break;
        // Resolution
        // TODO: Replace 0 with the clause id.
        proof->insert_top(literal, std::make_pair(0, Clause(nb_variables, clause)), std::make_pair(0, Clause(nb_variables, clause2)));
        clause.insert(clause2.begin(), clause2.end());
        mem_top--;
    }
    return std::shared_ptr<Clause>(new Clause(nb_variables, clause));
}

Affectation* satsolver::solve(Formula *formula) {
    int literal;
    bool with_proof;
    CLProof *proof;
    unsigned int clause_id;
    bool contains_false_clause;
    unsigned int skip_conflicts = 1; // Number of conflicts we will skip before showing another prompt
    int last_bet = 0; // Used for generating the graph.
    while(formula->get_aff()->get_nb_unknown() != 0 && !formula->only_true_clauses(NULL)) {
        formula->get_ded()->print() ;
        if(!WITH_WL && (literal = formula->monome(&clause_id))) {
            // We set the clause identified by “claused_id” as the one which
            // made us deduce the value of the literal.
            formula->deduce_true(literal, clause_id); // Return value ignored, WITH_WL is false
            contains_false_clause = formula->contains_false_clause(&clause_id);
        }
        else if((literal = formula->isolated_literal(&clause_id))) {
	        formula->deduce_true(literal,-1) ;
        }
        else {
            literal = formula->choose_literal(HEURISTIC) ;
            if (WITH_WL)
                contains_false_clause = !formula->bet_true(literal);
            else {
                formula->bet_true(literal);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
            last_bet = literal;
        }
        while(contains_false_clause) {
            // On met à jour la deduction "artificiellement" (ça n'impacte que la déduction, pas le reste de la formule)
            // Cette mise à jour sera annulée par le backtrack
            formula->get_ded()->add_deduction(literal, formula->to_clauses_vector()[clause_id]->whole_to_set());
            if (CL_INTERACT && --skip_conflicts == 0) {
                assert(last_bet);
                skip_conflicts = cl_interact(*formula->get_ded(), formula->get_aff(), last_bet, literal, &with_proof);
            }
            if (WITH_CL) {
                proof = new CLProof(formula->to_clauses_vector()[clause_id]);
                learn_clause(*formula->get_ded(), formula->get_mem(), formula->get_aff(), formula->get_nb_variables(), literal, proof);
                if (with_proof)
                    proof->to_latex_file(CL_PROOF_FILE_NAME);
                delete proof;
            }
            literal = formula->back() ;
            last_bet = -last_bet;
            if(literal == 0)
                throw Conflict() ;
            if (WITH_WL)
                contains_false_clause = !formula->deduce_false(literal,-1);
            else {
                formula->deduce_false(literal, -1);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
        }
    }
    return formula->get_aff() ;
}
