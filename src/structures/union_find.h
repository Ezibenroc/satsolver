#ifndef STRUCTURES_UNION_FIND_H
#define STRUCTURES_UNION_FIND_H

#include <list>
#include <stack>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

namespace theorysolver {

// A node in the union-find.
struct uf_node {
    uf_node(unsigned int v, struct uf_node *parent, unsigned int nb_childs) : v(v), parent(parent), edge_tag(0), nb_childs(nb_childs) {}
    unsigned int v;
    struct uf_node *parent;
    unsigned int edge_tag;
    unsigned int nb_childs;
};

// An object in the “pending merges” list.
struct node_or_pending_item {
    uf_node *node;
    std::list<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>>::iterator pending_item;

    node_or_pending_item(uf_node *node) : node(node), pending_item() {}
    node_or_pending_item(std::list<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>>::iterator pending_item) : node(NULL), pending_item(pending_item) {}
};


/* An implementation of union-find with no compression, but with taggable and
 * reversable unions */
class UnionFind {
    private:
        std::vector<uf_node*> nodes;
        std::stack<node_or_pending_item> merges; // A memory of previous merges, so they can be undone.

        std::list<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>> pending; // Merges between items in the same component
        
        void expand(unsigned int size);
        void compress(uf_node *node, uf_node *parent);

    public:
        UnionFind();
        UnionFind(unsigned int size_hint);
        ~UnionFind();

        void merge(unsigned int tag, unsigned int i, unsigned int j); // Merge two components
        unsigned int find(unsigned int i) const; // Get a representant of the component of a node
        void unmerge(); // Undo last merge
        std::unordered_set<int> get_path(unsigned int i); // Get a path from a not to the representant of its component

        std::string to_string() const;

        unsigned get_max_node() const; // Returns number of nodes.
};

}

#endif
