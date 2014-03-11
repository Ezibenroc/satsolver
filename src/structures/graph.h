#ifndef STRUCTURES_GRAPH_H
#define STRUCTURES_GRAPH_H

#include <set>

namespace graphsolver {

class Graph {
    private:
        int nodes_count;
        int *values;
        std::set<int> **adjacency;
        
    public:
        Graph(int nodes_count, int default_value);
        ~Graph();

        void set_value(int node_id, int value);
        void add_edge(int first, int second);

        int get_nodes_count();
        std::set<int>* get_lower_adjacent_nodes(int node_id);

};

}

#endif
