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

unsigned int cl_interac(const Deductions &deductions, const Affectation &aff, int last_bet, int literal, bool *with_proof) {
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
                return 1;
            case 'c':
                return 1;
            case 's':
                std::cin >> steps;
                return steps;
            case 't':
                CL_INTERACT = false;
                return 0;
            case 0:
                exit(0);
            default:
                std::cout << "Invalid character [g/r/c/s/t]. ";
                continue;
        }
    }
}



Affectation* satsolver::solve(Formula *formula) {
    int literal, literal_sav=0;
    bool with_proof;
    CLProof *proof;
    int clause_id, tmp;
    bool contains_false_clause;
    unsigned int skip_conflicts = 1; // Number of conflicts we will skip before showing another prompt
    int last_bet = 0; // Used for generating the graph.
    unsigned int depth_back ;
    while(formula->get_aff()->get_nb_unknown() != 0 && !formula->only_true_clauses(NULL)) {
        formula->get_ded()->print() ;
        if(!WITH_WL && (literal = formula->monome(&clause_id))) {
            // We set the clause identified by “claused_id” as the one which
            // made us deduce the value of the literal.
            formula->deduce_true(literal, clause_id,NULL,NULL,NULL);
            contains_false_clause = formula->contains_false_clause(&clause_id);
        }
        else if((literal = formula->isolated_literal(&clause_id))) {
            formula->deduce_true(literal,-1,NULL,NULL,NULL) ;
        }
        else {
            literal = formula->choose_literal(HEURISTIC) ;
            if (WITH_WL)
                contains_false_clause = !formula->bet_true(literal,&clause_id,&tmp,&literal);
            else {
                formula->bet_true(literal,NULL,NULL,NULL);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
        }
        while(contains_false_clause) {
            // On met à jour la deduction "artificiellement" (ça n'impacte que la déduction, pas le reste de la formule)
            // Cette mise à jour sera annulée par le backtrack
            // On sauvegarde avant l'indice de la clause ayant permi de déduire le littéral
            if(formula->get_ded_depth() == 0) { // clause fausse sans aucun paris, donc insatisfiable
                throw Conflict() ;
            }
            if(CL_INTERACT || WITH_CL) {
                if(!WITH_WL) { // formalités administratives un peu moches
                    tmp = formula->get_ded()->get_clause_id(literal) ;
                    formula->get_ded()->remove_deduction(literal);
                }
                formula->get_ded()->add_deduction(literal, formula->to_clauses_vector()[clause_id]->whole_to_set(),clause_id,formula->get_ded_depth());
                if(WITH_WL)
                    formula->get_aff()->set_true(literal) ;
                literal_sav = literal ;
                clause_id = tmp ;
                if(formula->to_clauses_vector()[clause_id]->get_size() <= 1 && formula->to_clauses_vector()[tmp]->get_size() <= 1)
                    throw Conflict() ;
            }
            with_proof = false;
            if (CL_INTERACT && --skip_conflicts == 0) {
                last_bet = formula->last_bet() ;
                assert(last_bet);
                skip_conflicts = cl_interac(*formula->get_ded(), formula->get_aff(), last_bet, literal, &with_proof);
            }
            if (WITH_CL) {
                proof = new CLProof();
                literal = formula->learn_clause(proof,&clause_id, &depth_back, literal);
                if (with_proof)
                    proof->to_latex_file(CL_PROOF_FILE_NAME);
                delete proof;
                formula->back(depth_back);
            }
            else {
                literal = -formula->back() ;
                clause_id = -1 ;
            }
            if(WITH_WL && (WITH_CL || CL_INTERACT)) { // nettoyage
                formula->get_ded()->remove_deduction(literal_sav) ;
                formula->get_aff()->set_unknown(literal_sav) ;
            }
            if(literal == 0)
                throw Conflict() ;
            if (WITH_WL)
                contains_false_clause = !formula->deduce_true(literal,clause_id,&clause_id,&tmp,&literal);
            else {
                formula->deduce_true(literal, clause_id,NULL,NULL,NULL);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
        }
    }
    return formula->get_aff() ;
}
