#include <set>
#include <map>
#include <iostream>
#include <cassert>
#include <memory>

#include <exception>
#include <stdexcept>

#include "structures/affectation.h"
#include "structures/formula.h"
#include "structures/clause.h"
#include "solvers/dpll.h"
#include "config.h"

#define CONFLICT_GRAPH_FILE_NAME "/tmp/conflict_graph.dot"

using namespace satsolver;

#define AS_AFF(a, l) (a.is_true(l) ? l : -l)

void handle_literal(FILE *graph_file, const std::map<int, std::set<int>> &deductions, std::set<int> *handled, std::set<int> &to_be_handled, const Affectation &aff, std::map<int, std::set<int>> &reversed_deductions, int literal) {
    // Add a literal to all reversed_deductions[i] sets if i was used to deduce the literal
    if (deductions.find(literal) == deductions.end())
        return;
    for (auto it : deductions.at(literal)) { // Iterate the literals which made us deduce the literal
        if (it == literal || it == -literal)
            continue;
        try {
            reversed_deductions[-it].insert(literal);
        }
        catch (std::out_of_range) {
            reversed_deductions[-it] = std::set<int>({literal});
        }
        to_be_handled.insert(it);
        if (handled->find(it) != handled->end() || handled->find(-it) != handled->end())
            continue;
        if (graph_file)
            fprintf(graph_file, "\t\"%d\" -> \"%d\";\n", AS_AFF(aff, it), AS_AFF(aff, literal));
    }
}


void conflict_graph_BFS(const std::map<int, std::set<int>> &reversed_deductions, int root, std::set<int> &unique_implication_points, std::set<int> &deduced_literals) {
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
                for (auto it2 : reversed_deductions.at(it))
                    next_depth.insert(it2);
            }
            catch (std::out_of_range) {
                // We reached a leaf.
            }
        }
    }
}

std::set<int>* make_conflict_graph(const std::map<int, std::set<int>> deductions, const Affectation &aff, int root, int literal, bool write) {
    FILE *graph_file = NULL;
    int literal2;
    std::set<int> *handled = new std::set<int>(), // The list of literals which have already been added to the graph
                  to_be_handled({literal}); // The list of literals in that will have to be added to the graph
    std::map<int, std::set<int>> reversed_deductions;
    std::set<int> unique_implication_points, deduced_literals;
    if (write) {
        graph_file = fopen(CONFLICT_GRAPH_FILE_NAME, "w");
        fprintf(graph_file, "digraph G {\n");
    }
    while (to_be_handled.size()) {
        literal2 = *to_be_handled.begin();
        to_be_handled.erase(to_be_handled.begin());
        if (handled->find(literal2) != handled->end() || handled->find(-literal2) != handled->end())
            continue;
        handled->insert(literal2);
        if (aff.is_unknown(abs(literal2)))
            continue;

        // The two following lines fill reversed_deductions and append items in to_be_handled.
        handle_literal(graph_file, deductions, handled, to_be_handled, aff, reversed_deductions, literal2);
        handle_literal(graph_file, deductions, handled, to_be_handled, aff, reversed_deductions, -literal2);
    }
    if (write) {
        // Fills unique_implication_points and deduced_literals
        conflict_graph_BFS(reversed_deductions, root, unique_implication_points, deduced_literals);
        for (auto it : deduced_literals)
            if (aff.is_true(it))
                fprintf(graph_file, "\t%d [color = \"blue\"];\n", it);
        // Note that UIPs are also deduced, so be both blue and yellow. Thanksfully,
        // dot deterministically chooses a color (the last one).
        for (auto it : unique_implication_points)
            if (aff.is_true(it))
                fprintf(graph_file, "\t%d [color = \"yellow\"];\n", it);
        fprintf(graph_file, "\t%d [color = \"red\"];\n", literal);
        fprintf(graph_file, "}\n");
        fclose(graph_file);
    }
    return handled;
}

unsigned int cl_interact(const std::map<int, std::set<int>> &deductions, const Affectation &aff, int last_bet, int literal) {
    char mode;
    unsigned int steps;
    assert(deductions.find(literal) != deductions.end() || deductions.find(-literal) != deductions.end());
    std::cout << "Conflict on literal " << literal << ". Action? [g/c/s/t] ";
    while (true) {
        std::cin >> mode;
        switch (mode) {
            case 'g':
                delete make_conflict_graph(deductions, aff, last_bet, literal, true);
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

std::shared_ptr<Clause> learn_clause(const std::map<int, std::set<int>> &deductions, const std::vector<std::pair<int, bool>> &mem, const Affectation &aff, int nb_variables, int literal) {
    (void) aff;
    long unsigned int mem_top = mem.size()-1; // The index in the memory “stack” of the literal we are making the resolution on.
    std::set<int> clause(deductions.at(literal)), clause2;
    assert(clause.find(-literal) != clause.end());
    mem_top--;
    while (mem.at(mem_top).second) {
        literal = mem.at(mem_top).first;
        assert(mem.at(mem_top).first == literal);
        try {
            clause2 = deductions.at(literal); // This is the clause we will make the resolution with
        }
        catch (std::out_of_range) {
            break;
        }

        if (clause.find(-literal) == clause.end())
            break;
        assert(clause2.find(literal) != clause.end());

        // Resolution
        clause.erase(clause.find(-literal));
        clause2.erase(clause2.find(literal));
        clause.insert(clause2.begin(), clause2.end());

        mem_top--;
    }
    return std::shared_ptr<Clause>(new Clause(nb_variables, clause));
}

Affectation* satsolver::solve(Formula *formula) {
    int literal;
    unsigned int clause_id;
    bool contains_false_clause;
    std::map<int, std::set<int>> deductions;
    unsigned int skip_conflicts = 1; // Number of conflicts we will skip before showing another prompt
    int last_bet = 0; // Used for generating the graph.
    while(formula->get_aff()->get_nb_unknown() != 0 && !formula->only_true_clauses(NULL)) {
        if(!WITH_WL && (literal = formula->monome(&clause_id))) {
            // We set the clause identified by “claused_id” as the one which
            // made us deduce the value of the literal.
            deductions[literal] = formula->to_clauses_vector()[clause_id]->whole_to_set();
            formula->deduce_true(literal, NULL); // Return value ignored, WITH_WL is false
            contains_false_clause = formula->contains_false_clause(&clause_id);
        }
        else if((literal = formula->isolated_literal(&clause_id))) {
            // We set the clause identified by “claused_id” as the one which
            // made us deduce the value of the literal.
            deductions[literal] = formula->to_clauses_vector()[clause_id]->whole_to_set();
/* // On ne créé pas de fausse clause en affectant un littéral isolé
	          if (WITH_WL)
                contains_false_clause = !formula->deduce_true(literal, &clause_id);
            else {
                formula->deduce_true(literal, NULL);
                contains_false_clause = formula->contains_false_clause(&clause_id);
            }
*/
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
            deductions[literal] = formula->to_clauses_vector()[clause_id]->whole_to_set();
            if (CL_INTERACT && --skip_conflicts == 0) {
                assert(last_bet);
                skip_conflicts = cl_interact(deductions, formula->get_aff(), last_bet, literal);
            }
            if (WITH_CL)
                learn_clause(deductions, formula->get_mem(), formula->get_aff(), formula->get_nb_variables(), literal);
            literal = formula->back() ;
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
