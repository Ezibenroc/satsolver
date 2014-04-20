#include <utility>
#include <fstream>
#include <iostream>

#include "structures/CL_proof.h"

#define MAX_TREE_DEPTH 4

CLProof::CLProof() : tree(), conclusion(0, std::unordered_set<int>()) {
}
CLProof::CLProof(const CLProof &that) : tree(that.tree), conclusion(0, std::unordered_set<int>()) {
}
CLProof& CLProof::operator=(const CLProof &that) {
    this->tree = that.tree;
    this->conclusion = that.conclusion;
    return *this;
}
CLProof::~CLProof() {
}

void CLProof::insert_top(int var, numbered_clause clause1, numbered_clause clause2) {
    this->tree.push_back(std::make_pair(var, std::make_pair(clause1, clause2)));
}
void CLProof::conclude(satsolver::Clause conclusion) {
    this->conclusion = conclusion;
}

#define print_tabs(file, level) for (i=0; i<level; i++) file << "\t";

void CLProof::to_latex_file(const std::string &filename) const {
    bool is_continuation;
    unsigned int i;
    unsigned int level = 0, proof_id=0;
    std::ofstream file(filename);
    bool first_line = true;
    file << "\\documentclass{article}\n\\usepackage{mathpartir}\n\\usepackage[utf8]{inputenc}\n\\newcommand{\\non}[1]{\\overline{#1}}\n\\newcommand{\\vart}[1]{x_{#1}}\n\\newcommand{\\varf}[1]{\\non{\\vart{#1}}}\n\\newcommand{\\cl}[1]{\\mathtt{C_{#1}}:~}\n\\newcommand{\\preuve}[1]{\\mathtt{\\Pi_{#1}}}\n\\begin{document}\n\nPreuve de résolution pour la clause :\n$$\n";
    file << this->conclusion.to_latex_string(0) << std::endl;
    file << "$$\n\n\\begin{mathpar}\n" << std::endl;
    auto it = this->tree.begin();
    for (; it != this->tree.end(); it++) {
        if ((it - this->tree.begin()) % MAX_TREE_DEPTH)
            is_continuation = false;
        else {
            is_continuation = (it != this->tree.begin());
            if (is_continuation)
                file << "{\n\t" << (it)->second.first.second.to_latex_string(0) << "\n}\n" << std::endl;
            file << "\n\\preuve{" << ++proof_id << "}:~ ";
            for (level=0; it+level<this->tree.end() && level<MAX_TREE_DEPTH; level++) {
                print_tabs(file, level);
                file << "\\inferrule{\n";
            }
        }
        if (!first_line) {
            print_tabs(file, level-1);
            file << "{\n";
        }
        print_tabs(file, level);
        if (is_continuation)
            file << "\\preuve{" << (proof_id-1) << "}";
        else
            file << it->second.first.second.to_latex_string(it->first);
        file << "\n";
        if (!first_line) {
            print_tabs(file, level-1);
            file << "}\n";
            level--;
        }
        print_tabs(file, level);
        file << "\\and\n";
        print_tabs(file, level);
        file << it->second.second.second.to_latex_string(it->first);
        file << "\n";
        if (level)
            print_tabs(file, level-1);
        file << "}\n";
        first_line = false;
    }
    file << "{\n\t" << this->conclusion.to_latex_string(0) << "\n}\n" << std::endl;
    file << "\\end{mathpar}\n\\end{document}" << std::endl;
}
