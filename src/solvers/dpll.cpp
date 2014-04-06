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

void write_conflict_graph(std::map<unsigned int, std::set<unsigned int>> deductions, unsigned int literal) {
    FILE *graph_file = fopen(CONFLICT_GRAPH_FILE_NAME, "w");
    std::set<unsigned int> handled, to_be_handled({literal});
    fprintf(graph_file, "digraph G {\n");
    while (to_be_handled.size()) {
        literal = *to_be_handled.begin();
        to_be_handled.erase(to_be_handled.begin());
        if (handled.find(literal) != handled.end())
            continue;
        handled.insert(literal);
        if (deductions.find(literal) != deductions.end()) {
            for (auto it : deductions.at(literal)) {
                if (it == literal)
                    continue;
                to_be_handled.insert(it);
                if (handled.find(it) != handled.end())
                    continue;
                fprintf(graph_file, "\t\"%u\" -> \"%u\";\n", it, literal);
            }
        }
    }
    fprintf(graph_file, "}\n");
    fclose(graph_file);
}

unsigned int cl_interact(std::map<unsigned int, std::set<unsigned int>> deductions, unsigned int literal) {
    char mode;
    unsigned int steps;
    assert(deductions.find(literal) != deductions.end());
    std::cout << "Conflict on literal " << literal << ". Action? [g/c/s/t] ";
    while (true) {
        std::cin >> mode;
        switch (mode) {
            case 'g':
                write_conflict_graph(deductions, literal);
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
    std::map<unsigned int, std::set<unsigned int>> deductions;
    unsigned int skip_conflicts = 1;
    while(formula->get_aff()->get_nb_unknown() != 0 && !formula->only_true_clauses(NULL)) {
        if(!WITH_WL && (literal = formula->monome(&clause_id))) {
            deductions[abs(literal)] = formula->to_clauses_vector()[clause_id]->variables_to_set();
            formula->deduce_true(literal, NULL); // Return value ignored, WITH_WL is false
            contains_false_clause = formula->contains_false_clause(&clause_id);
        }
        else if((literal = formula->isolated_literal(&clause_id))) {
            deductions[abs(literal)] = formula->to_clauses_vector()[clause_id]->variables_to_set();
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
            deductions[abs(literal)] = formula->to_clauses_vector()[clause_id]->variables_to_set();
            if (CL_INTERACT && --skip_conflicts == 0) {
                skip_conflicts = cl_interact(deductions, abs(literal));
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
