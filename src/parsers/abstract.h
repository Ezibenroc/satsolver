#ifndef PARSERS_ABSTRACT_H
#define PARSERS_ABSTRACT_H

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <exception>

namespace satsolver {

class AbstractParser {
    public:
        AbstractParser(std::istream &stream);
        virtual ~AbstractParser();

        class syntaxerror : public std::exception {
            private:
                const char *msg;
            public:
                syntaxerror(const char *_msg) : msg(_msg) {}
                syntaxerror(const std::string _msg) : msg(_msg.c_str()) {}
                syntaxerror() : msg("<unknown error>") {}
                virtual const char* what() const throw() {
                    std::cout << std::string(this->msg);
                    std::cout << std::endl;
                    return this->msg;
                }
        };

    protected:
        std::istream &stream;

        virtual void parse() = 0;
};

}

#endif
