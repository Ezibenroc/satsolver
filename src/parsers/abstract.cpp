#include "parsers/abstract.h"

AbstractParser::AbstractParser(std::istream &_stream) : stream(_stream) {
}
AbstractParser::~AbstractParser() {
}

