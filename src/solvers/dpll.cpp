#include <set>
#include <map>
#include <iostream>
#include <cassert>

#include <exception>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "solvers/dpll.h"
#include "config.h"

#define CONFLICT_GRAPH_FILE_NAME "/tmp/conflict_graph.dot"

using namespace satsolver;

#define AS_AFF(a, l) (aff.is_true(l) ? l : -l)

void handle_literal(FILE *graph_file, std::map<int, std::set<int>> deductions, std::set<int> *handled, std::set<int> &to_be_handled, Affectation &aff, int literal) {
    if (deductions.find(literal) == deductions.end())
        return;
    for (auto it : deductions.at(literal)) {
        if (it == literal || it == -literal)
            continue;
        to_be_handled.insert(it);
        if (handled->find(it) != handled->end() || handled->find(-it) != handled->end())
            continue;
        if (graph_file)
            fprintf(graph_file, "\t\"%d\" -> \"%d\";\n", AS_AFF(aff, it), AS_AFF(aff, literal));
    }
}

std::set<int>* make_conflict_graph(std::map<int, std::set<int>> deductions, Affectation &aff, int literal, bool write) {
    FILE *graph_file = NULL;
    std::set<int> *handled = new std::set<int>(), to_be_handled({literal});
    if (write) {
        graph_file = fopen(CONFLICT_GRAPH_FILE_NAME, "w");
        fprintf(graph_file, "digraph G {\n");
    }
    while (to_be_handled.size()) {
        literal = *to_be_handled.begin();
        to_be_handled.erase(to_be_handled.begin());
        if (handled->find(literal) != handled->end() || handled->find(-literal) != handled->end())
            continue;
        handled->insert(literal);
        assert(!aff.is_unknown(abs(literal)));
        handle_literal(graph_file, deductions, handled, to_be_handled, aff, literal);
        handle_literal(graph_file, deductions, handled, to_be_handled, aff, -literal);
    }
    if (write) {
        fprintf(graph_file, "}\n");
        fclose(graph_file);
    }
    return handled;
}

unsigned int cl_interact(std::map<int, std::set<int>> deductions, Affectation aff, int literal) {
    char mode;
    unsigned int steps;
    assert(deductions.find(literal) != deductions.end() || deductions.find(-literal) != deductions.end());
    std::cout << "Conflict on literal " << literal << ". Action? [g/c/s/t] ";
    while (true) {
        std::cin >> mode;
        switch (mode) {
            case 'g':
                make_conflict_graph(deductions, aff, literal, true);
                return 1;
            case 'c':
                return 1;
            case 's':
                std::cin >> steps;
                return steps;
            case 't':
                CL_INTERACT = false;
                return 0;
            default:
                std::cout << "Invalid character [g/c/s/t]. ";
                continue;
        }
    }
}

Affectation* satsolver::solve(Formula *formula) {
    int literal;
    unsigned int clause_id;
    bool contains_false_clause;
    std::map<int, std::set<int>> deductions;
    unsigned int skip_conflicts = 1;
    while(formula->get_aff()->get_nb_unknown() != 0 && !formula->only_true_clauses(NULL)) {
        if(!WITH_WL && (literal = formula->monome(&clause_id))) {
            deductions[literal] = formula->to_clauses_vector()[clause_id]->whole_to_set();
            formula->deduce_true(literal, NULL); // Return value ignored, WITH_WL is false
            contains_false_clause = formula->contains_false_clause(&clause_id);
        }
        else if((literal = formula->isolated_literal(&clause_id))) {
            deductions[literal] = formula->to_clauses_vector()[clause_id]->whole_to_set();
            if (WITH_WL)
                contains_false_clause = !formula->deduce_true(literal, &clause_id);
            else {
                formula->deduce_true(literal, NULL);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
        }
        else {
            literal = formula->choose_literal(HEURISTIC) ;
            if (WITH_WL)
                contains_false_clause = !formula->bet_true(literal, &clause_id);
            else {
                formula->bet_true(literal, NULL);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
        }
        while(contains_false_clause) {
            deductions[literal] = formula->to_clauses_vector()[clause_id]->whole_to_set();
            if (CL_INTERACT && --skip_conflicts == 0) {
                skip_conflicts = cl_interact(deductions, formula->get_aff(), literal);
            }
            literal = formula->back() ;
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
