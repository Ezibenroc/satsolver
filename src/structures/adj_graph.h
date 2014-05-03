#ifndef STRUCTURES_ADJ_GRAPH_H
#define STRUCTURES_ADJ_GRAPH_H

#include <list>
#include <vector>

namespace theorysolver {

typedef std::list<std::pair<unsigned int, int>> adj_list;

// A directed graph using adjacency lists, with taggable edges.
// worst case O(1) deletion, and access to edges
// amortized O(1) insertion of edges.
class AdjGraph {
    private:
        std::vector<adj_list> adj_lists;
        std::vector<std::vector<adj_list::iterator>> adj_list_links; // Pointers to items of adj_list (for O(1) deletion).

    public:
        AdjGraph();
        AdjGraph(unsigned int size_hint);

        void add_edge(unsigned int u, unsigned int v, int tag);
        void delete_edge(unsigned int u, unsigned int v);
        const adj_list& get_adj_list(unsigned int u) const;

        // Returns a path from a node to another with a minimum weight, if any
        // (returns an empty list otherwise)
        // Assumes there is no negative cycle in the graph.
        std::pair<std::list<unsigned int>, int> find_lowest_path(unsigned int from, unsigned int to) const;

};

}

#endif
