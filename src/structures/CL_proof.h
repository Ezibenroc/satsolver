#include <unordered_set>
#include <string>
#include <vector>
#include <memory>

#include "structures/clause.h"

class CLProof {
    satsolver::Clause root;
    std::vector<std::pair<satsolver::Clause, satsolver::Clause>> tree;

    public:
        CLProof();
        CLProof(satsolver::Clause &root);
        CLProof(std::shared_ptr<satsolver::Clause> root);
        CLProof(const CLProof &that);
        CLProof& operator=(const CLProof &that);
        ~CLProof();

        void insert_top(satsolver::Clause clause1, satsolver::Clause clause2);
        void to_latex_file(const std::string &filename) const;
};
