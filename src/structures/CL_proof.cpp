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

void CLProof::insert_top(satsolver::Clause clause1, satsolver::Clause clause2) {
    this->tree.push_back(std::make_pair(clause1, clause2));
}

void CLProof::to_latex_file(const std::string &filename) const {
    std::ofstream file(filename);
    file << "\\documentclass{article}\n\\usepackage{mathpartir}\n\\usepackage[utf8]{inputenc}\n\\newcommand{\\non}[1]{\\overline{#1}}\n\\newcommand{\\vart}[1]{x_{#1}}\n\\newcommand{\\varf}[1]{\\non{\\vart{#1}}}\n\\newcommand{\\cl}[1]{\\mathtt{C_{#1}}:~}\n\\newcommand{\\preuve}[1]{\\mathtt{\\Pi_{#1}}}\n\\begin{document}\n\nPreuve de résolution pour la clause :\n$$\n";
    file << this->root.to_latex_string() << std::endl;
    file << "$$" << std::endl;
    // TODO: Add the proof here
    file << "\\end{document}" << std::endl;
}
