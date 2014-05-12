#include <queue>
#include <cassert>
#include <climits>
#include <sstream>
#include <iostream>

#include "structures/adj_graph.h"
#include "config.h"

#define max(u, v) ((u > v) ? (u) : (v))

using namespace theorysolver;

AdjGraph::AdjGraph() : adj_lists(), adj_list_links() {
}

AdjGraph::AdjGraph(unsigned int size_hint) : adj_lists(size_hint), adj_list_links() {
}

void AdjGraph::add_edge(unsigned int u, unsigned int v, unsigned int tag, int weight) {
    struct adj_list_item item;
    if (max(u, v) >= this->adj_lists.size())
        this->adj_lists.resize(max(u, v)+1);
    if (tag >= this->adj_list_links.size())
        this->adj_list_links.resize(tag+1, std::make_pair(false, this->adj_lists[u].begin()));
    item.v = v;
    item.tag = tag;
    item.weight = weight;
    this->adj_lists[u].push_front(item);
    this->adj_list_links[tag] = make_pair(true, this->adj_lists[u].begin());
}

void AdjGraph::delete_edge(unsigned int u, unsigned int v, unsigned int tag) {
    assert(tag<this->adj_list_links.size());
    assert(this->adj_list_links[tag].first);
    (void) v;
    this->adj_list_links[tag].first = false;
    this->adj_lists[u].erase(this->adj_list_links[tag].second);
}

const adj_list& AdjGraph::get_adj_list(unsigned int u) const {
    return this->adj_lists[u];
}


int AdjGraph::get_weight(unsigned int tag) const {
    assert(tag < this->adj_list_links.size());
    assert(this->adj_list_links[tag].first);
    return this->adj_list_links[tag].second->weight;
}

std::pair<std::list<path_item>, int> AdjGraph::find_lowest_path(unsigned int from, unsigned int to) const {
    unsigned int u;
    unsigned long int nb_nodes = max(max(from, to)+1, this->adj_lists.size()+1);
    std::vector<int> distances(nb_nodes, INT_MAX);
    std::vector<std::list<path_item>> paths(nb_nodes);
    std::queue<unsigned int> todo({from});
    struct path_item item;
    std::vector<bool> in_todo(nb_nodes, false);
    distances[from] = 0;
    in_todo[from] = true;

    while (todo.size()) {
        u = todo.front();
        todo.pop();
        in_todo[u] = false;
        assert(distances[u] < INT_MAX);
        if (u>=this->adj_lists.size())
            continue;
        for (auto it : this->get_adj_list(u)) {
            if (distances[it.v] > distances[u]+it.weight) {
                distances[it.v] = distances[u]+it.weight;
                assert(it.v < paths.size());
                paths[it.v] = paths[u];
                item.node = it.v;
                item.tag = it.tag;
                item.weight = it.weight;
                paths[it.v].push_back(item);
                if (!in_todo[it.v]) {
                    todo.push(it.v);
                    in_todo[it.v] = true;
                }
            }
        }
    }
    return std::make_pair(paths[to], distances[to]);
}

std::string AdjGraph::to_string() const {
    std::ostringstream oss;
    for (unsigned int i=0; i<this->adj_lists.size(); i++) {
        for (auto it : this->adj_lists[i]) {
            oss << i << " -> " << it.v << " (" << it.tag << ", " << it.weight << ")\n";
        }
    }
    return oss.str();
}
