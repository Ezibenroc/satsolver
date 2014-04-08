#include <cassert>
#include <cstdlib>
#include <cstring>

#include "structures/graph.h"

using namespace graphsolver;

Graph::Graph(int nodes_count, int default_value) : nodes_count(nodes_count), values(static_cast<int*>(malloc(sizeof(int)*nodes_count))), adjacency(static_cast<std::set<int>**>(malloc(sizeof(std::set<int>*)*nodes_count))) {
    int i;
    memset(values, default_value, nodes_count);
    for (i=0; i<nodes_count; i++) {
        this->adjacency[i] = new std::set<int>();
    }
}
graphsolver::Graph::Graph(const graphsolver::Graph &that) : nodes_count(that.nodes_count), values(static_cast<int*>(malloc(sizeof(int)*nodes_count))), adjacency(static_cast<std::set<int>**>(malloc(sizeof(std::set<int>*)*nodes_count))) {
    int i;
    memcpy(values, that.values, nodes_count);
    for (i=0; i<nodes_count; i++) {
        this->adjacency[i] = new std::set<int>(*that.adjacency[i]);
    }
}

graphsolver::Graph& graphsolver::Graph::operator=(const graphsolver::Graph &that) {
    int i;
    free(this->values);
    for (i=0; i<this->nodes_count; i++) {
        delete this->adjacency[i];
    }
    free(this->adjacency);

    this->nodes_count = that.nodes_count;
    values = static_cast<int*>(malloc(sizeof(int)*that.nodes_count));
    adjacency = static_cast<std::set<int>**>(malloc(sizeof(std::set<int>*)*nodes_count));
    memcpy(values, that.values, nodes_count);
    for (i=0; i<nodes_count; i++) {
        this->adjacency[i] = new std::set<int>(*that.adjacency[i]);
    }
    return *this;
}

Graph::~Graph() {
    int i;
    free(this->values);
    
    for (i=0; i<this->nodes_count; i++) {
        delete this->adjacency[i];
    }
    free(this->adjacency);
}

void Graph::set_value(int node_id, int value) {
    assert(node_id > 0);
    assert(node_id <= this->nodes_count); 
    this->values[node_id-1] = value;
}

void Graph::add_edge(int first, int second) {
    assert(first < this->nodes_count);
    assert(second < this->nodes_count);
    assert(first != second);
    if (first > second)
        this->adjacency[first]->insert(second);
    else
        this->adjacency[second]->insert(first);
}

int Graph::get_nodes_count() {
    return this->nodes_count;
}

std::set<int>* Graph::get_lower_adjacent_nodes(int node_id) {
    assert(node_id < this->nodes_count);
    return this->adjacency[node_id];
}
