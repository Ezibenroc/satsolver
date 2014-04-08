#include <cassert>

#include "parsers/graph.h"

using namespace graphsolver;

GraphParser::GraphParser(std::istream &stream) : AbstractParser(stream), current_node_id(-1), nodes_count(0), edges_count(0), graph((Graph*) NULL) {
}
GraphParser::GraphParser(const GraphParser &that) : AbstractParser(that), current_node_id(that.current_node_id), nodes_count(that.nodes_count), edges_count(that.edges_count), graph(new Graph(*that.graph)) {
}
GraphParser& GraphParser::operator=(const GraphParser &that) {
    this->current_node_id = that.current_node_id;
    this->nodes_count = that.nodes_count;
    this->edges_count = that.edges_count;
    this->graph = new Graph(*that.graph);
    return *this;
}
GraphParser::~GraphParser() {
}

void GraphParser::parse() {
    enum GraphParser::State state = WAITING_FOR_HEADER;
    while (state != GraphParser::END_OF_FILE)
        state = this->transition(state);
}

enum GraphParser::State GraphParser::transition(enum GraphParser::State state) {
    char prefix;
    std::string input_string;
    int input_integer;
    switch (state) {

        case WAITING_FOR_HEADER: // At the beginning of a line, header not seen yet
            this->stream >> prefix;
            switch (prefix) {
                case 'c':
                    return IN_COMMENT_BEFORE_HEADER;
                case 'p':
                    return WAITING_FOR_FORMAT;
                case ' ':
                    return WAITING_FOR_HEADER;
                default:
                    throw GraphParser::syntaxerror("Expected header.");
            };
            break;

        case WAITING_FOR_FORMAT: // In the header line, “p” has been read
            this->stream >> input_string;
            if (input_string != "edge")
                throw GraphParser::syntaxerror("Expected 'edge'.");
            return WAITING_FOR_NODES_COUNT;

        case WAITING_FOR_NODES_COUNT: // In the header line, “p edge” has been read
            this->stream >> this->nodes_count;
            this->graph = new Graph(this->nodes_count, 1);
            return WAITING_FOR_EDGES_COUNT;

        case WAITING_FOR_EDGES_COUNT: // In the header line, “p edge X” (where X is the nodes count) has been read
            this->stream >> this->edges_count;
            return WAITING_FOR_LINE;

        case WAITING_FOR_LINE: // At the beginning of a line, header has already been seen.
            if (this->stream.eof())
                return END_OF_FILE;
            this->stream >> input_string;
            switch (input_string[0]) {
                case 'n':
                    return WAITING_FOR_NODE_ID;
                case 'e':
                    return WAITING_FOR_EDGE_FIRST;
                case 'd':
                    return WAITING_FOR_OPTIONAL_DESCRIPTOR_FIRST;
                case 'c':
                    return IN_COMMENT;
                case 0:
                case '\n':
                    return WAITING_FOR_LINE;
                default:
                    throw GraphParser::syntaxerror("Unknown descriptor type.");
            }
            assert(false); // We should never be here

        case WAITING_FOR_NODE_ID:
            assert(this->current_node_id == -1);
            this->stream >> this->current_node_id;
            return WAITING_FOR_NODE_VALUE;

        case WAITING_FOR_NODE_VALUE:
            this->stream >> input_integer;
            this->graph->set_value(this->current_node_id-1, input_integer);
            this->current_node_id = -1;
            return WAITING_FOR_LINE;

        case WAITING_FOR_EDGE_FIRST:
            assert(this->current_node_id == -1);
            this->stream >> this->current_node_id;
            return WAITING_FOR_EDGE_SECOND;

        case WAITING_FOR_EDGE_SECOND:
            this->stream >> input_integer;
            this->graph->add_edge(this->current_node_id-1, input_integer-1);
            this->current_node_id = -1;
            return WAITING_FOR_LINE;

        case WAITING_FOR_OPTIONAL_DESCRIPTOR_FIRST:
            this->stream >> input_string;
            return WAITING_FOR_OPTIONAL_DESCRIPTOR_SECOND;

        case WAITING_FOR_OPTIONAL_DESCRIPTOR_SECOND:
            this->stream >> input_string;
            return WAITING_FOR_LINE;

        case IN_COMMENT: // In a comment. Header has already been seen.
            std::getline(this->stream, input_string);
            return WAITING_FOR_LINE;

        case IN_COMMENT_BEFORE_HEADER: // In a command. Header has not been seen yet.
            std::getline(this->stream, input_string);
            return WAITING_FOR_HEADER;

        case END_OF_FILE:
            // We have read the EOF character and set the state to END_OF_FILE, which is a terminal state and therefore this case should never be matched.
            assert(false);
    }
    assert(false);
    return END_OF_FILE;
}

Graph* GraphParser::get_graph() {
    return this->graph;
}
