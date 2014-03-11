#ifndef PARSERS_GRAPH_H
#define PARSERS_GRAPH_H

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <exception>

#include "structures/graph.h"
#include "parsers/abstract.h"

namespace graphsolver {

// Finite state machine
class GraphParser : public AbstractParser {
    public:
        enum State {
            WAITING_FOR_HEADER,
            WAITING_FOR_FORMAT,
            WAITING_FOR_NODES_COUNT,
            WAITING_FOR_EDGES_COUNT,

            WAITING_FOR_LINE,

            WAITING_FOR_NODE_ID,
            WAITING_FOR_NODE_VALUE,

            WAITING_FOR_EDGE_FIRST,
            WAITING_FOR_EDGE_SECOND,

            WAITING_FOR_OPTIONAL_DESCRIPTOR_FIRST,
            WAITING_FOR_OPTIONAL_DESCRIPTOR_SECOND,

            IN_COMMENT,
            IN_COMMENT_BEFORE_HEADER,

            END_OF_FILE
        };

    private:

        int current_node_id;

        int nodes_count, edges_count;
        Graph *graph;

        enum State transition(enum State);


    public:
        GraphParser(std::istream &stream);
        virtual ~GraphParser();
        virtual void parse();

        Graph* get_graph();
};

}

#endif
