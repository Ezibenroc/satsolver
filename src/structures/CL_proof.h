#ifndef STRUCTURES_CLPROOF_H
#define STRUCTURES_CLPROOF_H

#include <unordered_set>
#include <string>
#include <vector>
#include <memory>

#include "structures/clause.h"

#define numbered_clause std::pair<int, satsolver::Clause>

class CLProof {
    satsolver::Clause root;
    std::vector<std::pair<int, std::pair<numbered_clause, numbered_clause>>> tree;

    public:
        CLProof();
        CLProof(satsolver::Clause &root);
        CLProof(std::shared_ptr<satsolver::Clause> root);
        CLProof(const CLProof &that);
        CLProof& operator=(const CLProof &that);
        ~CLProof();

        void insert_top(int literal, numbered_clause clause1, numbered_clause clause2);
        void to_latex_file(const std::string &filename) const;
};

#endif
