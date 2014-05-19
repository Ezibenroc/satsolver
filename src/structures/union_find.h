#ifndef STRUCTURES_UNION_FIND_H
#define STRUCTURES_UNION_FIND_H

#include <stack>
#include <string>
#include <vector>
#include <unordered_set>

namespace theorysolver {

struct uf_node {
    uf_node(unsigned int v, struct uf_node *parent, unsigned int nb_childs) : v(v), parent(parent), edge_tag(0), nb_childs(nb_childs) {}
    unsigned int v;
    struct uf_node *parent;
    int edge_tag;
    unsigned int nb_childs;
};

/* An implementation of union-find with no compression, but with taggable and
 * reversable unions */
class UnionFind {
    private:
        std::vector<uf_node*> nodes;
        std::stack<uf_node*> merges; // (clause_id, atom)
        
        void expand(unsigned int size);
        void compress(uf_node *node, uf_node *parent);

    public:
        UnionFind();
        UnionFind(unsigned int size_hint);

        void merge(int tag, unsigned int i, unsigned int j);
        unsigned int find(unsigned int i);
        unsigned int unmerge();
        std::unordered_set<int> get_path(unsigned int i);

        std::string to_string() const;
};

}

#endif
