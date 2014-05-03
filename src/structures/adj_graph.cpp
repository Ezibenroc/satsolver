#include <cassert>

#include "structures/adj_graph.h"

#define max(u, v) ((u > v) ? (u) : (v))

using namespace theorysolver;

AdjGraph::AdjGraph() : adj_lists(), adj_list_links() {
}

AdjGraph::AdjGraph(unsigned int size_hint) : AdjGraph() {
    this->adj_lists.resize(size_hint);
    this->adj_list_links.resize(size_hint);
}

void AdjGraph::add_edge(unsigned int u, unsigned int v, int tag) {
    assert(this->adj_lists.size() == this->adj_list_links.size());
    if (max(u, v) >= this->adj_lists.size()) {
        this->adj_lists.resize(max(u, v));
        this->adj_list_links.resize(max(u, v));
    }
    this->adj_lists[u].push_front(std::make_pair(v, tag));
    if (v >= this->adj_list_links[u].size())
        this->adj_list_links[u].resize(v+1);
    this->adj_list_links[u][v] = this->adj_lists[u].begin();
}

void AdjGraph::delete_edge(unsigned int u, unsigned int v) {
    this->adj_lists[u].erase(this->adj_list_links[u][v]);
}

const adj_list& AdjGraph::get_adj_list(unsigned int u) const {
    return this->adj_lists[u];
}
