#include <queue>
#include <cassert>
#include <climits>
#include <iostream>

#include "structures/adj_graph.h"

#define max(u, v) ((u > v) ? (u) : (v))

using namespace theorysolver;

AdjGraph::AdjGraph() : adj_lists(), adj_list_links() {
}

AdjGraph::AdjGraph(unsigned int size_hint) : adj_lists(size_hint), adj_list_links(size_hint) {
}

void AdjGraph::add_edge(unsigned int u, unsigned int v, int tag) {
    assert(this->adj_lists.size() == this->adj_list_links.size());
    if (max(u, v) >= this->adj_lists.size()) {
        this->adj_lists.resize(max(u, v)+1);
        this->adj_list_links.resize(max(u, v)+1);
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

std::pair<std::list<unsigned int>, int> AdjGraph::find_lowest_path(unsigned int from, unsigned int to) const {
    unsigned int u;
    unsigned long int nb_nodes = this->adj_lists.size();
    std::vector<int> distances(nb_nodes, INT_MAX);
    std::vector<std::list<unsigned int>> paths(nb_nodes);
    std::queue<unsigned int> todo({from});
    std::vector<bool> in_todo(nb_nodes, false);
    distances[from] = 0;
    in_todo[from] = true;

    while (todo.size()) {
        u = todo.front();
        todo.pop();
        in_todo[u] = false;
        assert(distances[u] < INT_MAX);
        for (auto it : this->get_adj_list(u)) {
            if (distances[it.first] > distances[u]+it.second) {
                distances[it.first] = distances[u]+it.second;
                assert(it.first < paths.size());
                paths[it.first] = paths[u];
                paths[it.first].push_back(it.first);
                if (!in_todo[it.first]) {
                    todo.push(it.first);
                    in_todo[it.first] = true;
                }
            }
        }
    }
    return std::make_pair(paths[to], distances[to]);
}
