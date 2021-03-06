#include <cassert>

#include "structures/union_find.h"

using namespace theorysolver;

UnionFind::UnionFind() : nodes(), merges(), pending() {
}

UnionFind::UnionFind(unsigned int size_hint) : nodes(), merges(), pending() {
    this->expand(size_hint);
}

UnionFind::~UnionFind() {
    for (unsigned i=0; i<this->nodes.size(); i++)
        delete this->nodes.at(i);
}

void UnionFind::expand(unsigned int size) {
    if (size < this->nodes.size())
        return;
    nodes.reserve(size);
    for (unsigned i=static_cast<unsigned>(this->nodes.size()); i<size; i++)
        this->nodes.push_back(new uf_node(i, NULL, 0));
}


void UnionFind::merge(unsigned int tag, unsigned int i, unsigned j) {
    struct uf_node *node;
    if (j < i) // Make sure there is no cycle.
        return this->merge(tag, j, i);
    if (this->find(i) == this->find(j)) { // The two nodes are already in the same component
        this->pending.push_front(std::make_pair(tag, std::make_pair(i, j)));
        this->merges.push(node_or_pending_item(this->pending.begin()));
        return;
    }
    this->expand(j+1);
    node = this->nodes[j];
    while (node->parent)
        node = node->parent;
    node->parent = this->nodes[i];
    node->edge_tag = tag;
    this->nodes[i]->nb_childs++;
    this->merges.push(node_or_pending_item(node));
}

unsigned int UnionFind::find(unsigned int i) const {
    uf_node *node;
    if (i>=this->nodes.size())
        return i;
    node = this->nodes[i];
    while (node->parent)
        node = node->parent;
    return node->v;
}
std::unordered_set<int> UnionFind::get_path(unsigned int i) {
    std::unordered_set<int> path;
    uf_node *node;
    assert(i<this->nodes.size());
    node = this->nodes[i];
    assert(node);
    while (node->parent) {
        path.insert(node->edge_tag);
        node = node->parent;
    }
    return path;
}
void UnionFind::unmerge() {
    std::list<std::pair<unsigned int, std::pair<unsigned int, unsigned int>>>::iterator it, it2;
    uf_node *node;
    assert(this->merges.size());
    node = this->merges.top().node;
    if (node) { // The last merge is between different components
        this->merges.pop();
        node->parent = NULL;

        // Apply a pending merge, if any.
        it=this->pending.begin();
        while (it!=this->pending.end()) {
            it2 = ++it;
            it--;
            if (this->find(it->second.first) != this->find(it->second.second)) {
                this->merge(it->first, it->second.first, it->second.second);
                this->pending.erase(it);
                break;
            }
            it = it2;
        }
    }
    else { // The last merge is between nodes of the same component
        this->pending.erase(this->merges.top().pending_item);
        this->merges.pop();
    }
}

unsigned UnionFind::get_max_node() const {
    return static_cast<unsigned>(this->nodes.size());
}
