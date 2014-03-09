#include "parsers/abstract.h"

using namespace satsolver;

AbstractParser::AbstractParser(std::istream &_stream) : stream(_stream) {
}
AbstractParser::~AbstractParser() {
}

