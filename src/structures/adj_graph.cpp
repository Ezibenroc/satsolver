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

AdjGraph::AdjGraph(unsigned int size_hint) : adj_lists(size_hint), adj_list_links(size_hint) {
}

void AdjGraph::add_edge(unsigned int u, unsigned int v, int tag, int weight) {
    assert(this->adj_lists.size() == this->adj_list_links.size());
    if (max(u, v) >= this->adj_lists.size()) {
        this->adj_lists.resize(max(u, v)+1);
        this->adj_list_links.resize(max(u, v)+1);
    }
    this->adj_lists[u].push_front(std::make_pair(v, std::make_pair(tag, weight)));
    if (v >= this->adj_list_links[u].size())
        this->adj_list_links[u].resize(v+1);
    this->adj_list_links[u][v] = make_pair(true, this->adj_lists[u].begin());
}

void AdjGraph::delete_edge(unsigned int u, unsigned int v) {
    assert(this->adj_lists.size() == this->adj_list_links.size());
    if (u>=this->adj_list_links.size() || v>=this->adj_list_links[u].size())
        return;
    if (this->adj_list_links[u][v].first) {
        this->adj_list_links[u][v].first = false;
        this->adj_lists[u].erase(this->adj_list_links[u][v].second);
    }
    else if (VERBOSE)
        std::cout << "Trying to delete a non-existing edge…" << std::endl;
}

const adj_list& AdjGraph::get_adj_list(unsigned int u) const {
    return this->adj_lists[u];
}


int AdjGraph::get_weight(unsigned int u, unsigned int v) const {
    assert(this->adj_lists.size() == this->adj_list_links.size());
    assert(u<this->adj_list_links.size() && v<this->adj_list_links[u].size());
    assert(this->adj_list_links[u][v].first);
    return this->adj_list_links[u][v].second->second.second;
}

std::pair<std::list<std::pair<unsigned int, int>>, int> AdjGraph::find_lowest_path(unsigned int from, unsigned int to) const {
    unsigned int u;
    unsigned long int nb_nodes = max(max(from, to)+1, this->adj_lists.size()+1);
    std::vector<int> distances(nb_nodes, INT_MAX);
    std::vector<std::list<std::pair<unsigned int, int>>> paths(nb_nodes);
    std::queue<unsigned int> todo({from});
    std::vector<bool> in_todo(nb_nodes, false);
    assert(this->adj_lists.size() == this->adj_list_links.size());
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
            if (distances[it.first] > distances[u]+it.second.second) {
                distances[it.first] = distances[u]+it.second.second;
                assert(it.first < paths.size());
                paths[it.first] = paths[u];
                paths[it.first].push_back(std::make_pair(it.first, it.second.first));
                if (!in_todo[it.first]) {
                    todo.push(it.first);
                    in_todo[it.first] = true;
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
            oss << i << " -> " << it.first << " (" << it.second.first << ", " << it.second.second << ")\n";
        }
    }
    return oss.str();
}
