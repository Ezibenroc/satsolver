#include <utility>
#include <fstream>

#include "structures/CL_proof.h"

CLProof::CLProof() : root(satsolver::Clause(0, std::unordered_set<int>({}))), tree() {
}
CLProof::CLProof(satsolver::Clause &root) : root(root), tree() {
}
CLProof::CLProof(std::shared_ptr<satsolver::Clause> root) : root(*root), tree() {
}
CLProof::CLProof(const CLProof &that) : root(that.root), tree(that.tree) {
}
CLProof& CLProof::operator=(const CLProof &that) {
    this->tree = that.tree;
    return *this;
}
CLProof::~CLProof() {
}

void CLProof::insert_top(int var, numbered_clause clause1, numbered_clause clause2) {
    this->tree.push_back(std::make_pair(var, std::make_pair(clause1, clause2)));
}

#define print_tabs(file, level) for (i=0; i<level; i++) file << "\t";

void CLProof::to_latex_file(const std::string &filename) const {
    unsigned int i;
    unsigned int level = 0;
    std::ofstream file(filename);
    bool first_line = true;
    file << "\\documentclass{article}\n\\usepackage{mathpartir}\n\\usepackage[utf8]{inputenc}\n\\newcommand{\\non}[1]{\\overline{#1}}\n\\newcommand{\\vart}[1]{x_{#1}}\n\\newcommand{\\varf}[1]{\\non{\\vart{#1}}}\n\\newcommand{\\cl}[1]{\\mathtt{C_{#1}}:~}\n\\newcommand{\\preuve}[1]{\\mathtt{\\Pi_{#1}}}\n\\begin{document}\n\nPreuve de résolution pour la clause :\n$$\n";
    file << this->root.to_latex_string(0) << std::endl;
    file << "$$\n\n\\begin{mathpar}\n\\preuve{1}:~ " << std::endl;
    for (level=0; level<this->tree.size(); level++) {
        print_tabs(file, level);
        file << "\\inferrule{\n";
    }
    for (auto it : this->tree) {
        if (!first_line) {
            print_tabs(file, level-1);
            file << "{\n";
        }
        print_tabs(file, level);
        file << it.second.first.second.to_latex_string(it.first);
        file << "\n";
        if (!first_line) {
            print_tabs(file, level-1);
            file << "}\n";
            level--;
        }
        print_tabs(file, level);
        file << "\\and\n";
        print_tabs(file, level);
        file << it.second.second.second.to_latex_string(it.first);
        file << "\n";
        print_tabs(file, level-1);
        file << "}\n";
        first_line = false;
    }
    file << "{\n\t" << this->root.to_latex_string(0) << "\n}\n" << std::endl;
    file << "\\end{mathpar}\n\\end{document}" << std::endl;
}
