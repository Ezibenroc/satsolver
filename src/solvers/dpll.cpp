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

#define CONFLICT_GRAPH_FILE_NAME "/tmp/conflict_graph.dot"
#define CL_PROOF_FILE_NAME "/tmp/CL_proof.tex"

using namespace satsolver;

#define AS_AFF(a, l) (a.is_true(l) ? l : -l)

void handle_literal(FILE *graph_file, const Deductions &deductions, std::set<int> &to_be_handled, const Affectation &aff, int literal) {
    if (!deductions.has_literal(literal)) {
        return;
   	}
    for (auto it : deductions.get_deduced_from(literal)) { // Iterate the literals which made us deduce the literal
        if (it == literal || it == -literal)
            continue;
        to_be_handled.insert(it);
        if (graph_file)
            fprintf(graph_file, "\t\"%d\" -> \"%d\";\n", AS_AFF(aff, it), AS_AFF(aff, literal));
    }
}


void conflict_graph_BFS(const Deductions &deductions, int root, std::set<int> &unique_implication_points, std::set<int> &deduced_literals) {
    // We will browse the graph layer after layer from the root.
    // If a layer is a singleton, this element is a unique implication point.
    std::set<int> current_depth, next_depth({root});
    while (next_depth.size()) {
        current_depth.clear();
        current_depth.swap(next_depth);
        if (current_depth.size() == 1)
            unique_implication_points.insert(*current_depth.begin());
        for (auto it : current_depth) {
            deduced_literals.insert(it);
            try {
                for (auto it2 : deductions.get_deductions(-it))
                    if (it2 != it)
                        next_depth.insert(it2);
            }
            catch (std::out_of_range) {
                // We reached a leaf.
            }
        }
    }
}

void make_conflict_graph(const Deductions &deductions, const Affectation &aff, int root, int literal) {
    FILE *graph_file = NULL;
    //int literal2;
    std::set<int> to_be_handled({literal}); // The list of literals in that will have to be added to the graph
    std::set<int> unique_implication_points, deduced_literals;
    
    graph_file = fopen(CONFLICT_GRAPH_FILE_NAME, "w");
    fprintf(graph_file, "digraph G {\n");
    
    // Arêtes
    deductions.print_edges(graph_file,aff) ;
    
    // Fills unique_implication_points and deduced_literals
    conflict_graph_BFS(deductions, root, unique_implication_points, deduced_literals);
    for (auto it : deduced_literals)
        if (aff.is_true(it))
            fprintf(graph_file, "\t%d [color = \"blue\"];\n", it);
    
    // Noeuds
    deductions.print_UIP(graph_file,aff,root,literal) ;
    fprintf(graph_file, "\t%d [color = \"red\"];\n", literal);
    fprintf(graph_file, "}\n");
    fclose(graph_file);
}

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
                make_conflict_graph(deductions, aff, last_bet, literal);
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
    std::unordered_set<int> clause(deductions.get_deduced_from(literal)), clause2;
    assert(clause.find(-literal) != clause.end());
    mem_top--;
    while (mem.at(mem_top).second) {
        literal = mem.at(mem_top).first;
        assert(mem.at(mem_top).first == literal);
        try {
            clause2 = deductions.get_deduced_from(literal); // This is the clause we will make the resolution with
        }
        catch (std::out_of_range) {
            break;
        }

        if (clause.find(-literal) == clause.end())
            break;
        assert(clause2.find(literal) != clause.end());
        // Resolution
        // TODO: Replace 0 with the clause id.
        proof->insert_top(literal, std::make_pair(0, Clause(nb_variables, clause)), std::make_pair(0, Clause(nb_variables, clause2)));
        clause.erase(clause.find(-literal));
        clause2.erase(clause2.find(literal));
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
    Deductions *deductions;
    unsigned int skip_conflicts = 1; // Number of conflicts we will skip before showing another prompt
    int last_bet = 0; // Used for generating the graph.
    deductions = formula->get_ded() ;
    while(formula->get_aff()->get_nb_unknown() != 0 && !formula->only_true_clauses(NULL)) {
        deductions->print() ;
        if(!WITH_WL && (literal = formula->monome(&clause_id))) {
            // We set the clause identified by “claused_id” as the one which
            // made us deduce the value of the literal.
            deductions->add_deduction(literal, formula->to_clauses_vector()[clause_id]->whole_to_set());
            formula->deduce_true(literal, NULL); // Return value ignored, WITH_WL is false
            contains_false_clause = formula->contains_false_clause(&clause_id);
        }
        else if((literal = formula->isolated_literal(&clause_id))) {
            // We set the clause identified by “claused_id” as the one which
            // made us deduce the value of the literal.
            deductions->add_deduction(literal, formula->to_clauses_vector()[clause_id]->whole_to_set());
						formula->deduce_true(literal,NULL) ;
        }
        else {
            literal = formula->choose_literal(HEURISTIC) ;
            if (WITH_WL)
                contains_false_clause = !formula->bet_true(literal, &clause_id);
            else {
                formula->bet_true(literal, NULL);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
            last_bet = literal;
        }
        while(contains_false_clause) {
            // We set the clause identified by “claused_id” as the one which
            // made us deduce the value of the literal.
            deductions->add_deduction(literal, formula->to_clauses_vector()[clause_id]->whole_to_set());
            if (CL_INTERACT && --skip_conflicts == 0) {
                assert(last_bet);
                skip_conflicts = cl_interact(*deductions, formula->get_aff(), last_bet, literal, &with_proof);
            }
            if (WITH_CL) {
                proof = new CLProof(formula->to_clauses_vector()[clause_id]);
                learn_clause(*deductions, formula->get_mem(), formula->get_aff(), formula->get_nb_variables(), literal, proof);
                if (with_proof)
                    proof->to_latex_file(CL_PROOF_FILE_NAME);
                delete proof;
            }
            literal = formula->back() ;
            deductions->remove_unknown(*formula->get_aff());
            last_bet = -last_bet;
            if(literal == 0)
                throw Conflict() ;
            if (WITH_WL)
                contains_false_clause = !formula->deduce_false(literal, &clause_id);
            else {
                formula->deduce_false(literal, NULL);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
        }
    }
    return formula->get_aff() ;
}
