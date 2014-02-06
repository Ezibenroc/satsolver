#include <iostream>
#include <string>
#include "structures.h"
#include <vector>

using namespace satsolver;
int main(void) {
	std::vector <int> v = {1,2,-3,4,} ;
	Clause *c = new Clause(4,v) ;
	std::cout << c->to_string() << std::endl ;
	c->add(-2) ;
	std::cout << c->to_string() << std::endl ;
	c->add(3) ;
	Clause *a = new Clause(*c) ;
	std::cout << c->to_string() << std::endl ;
	c->remove(-1) ;c->remove(1) ; c->remove(2) ; c->remove(-2) ; c->remove(-3) ;
	std::cout << c->to_string() << std::endl ;
	c->remove(3) ;	c->remove(4) ;
	std::cout << c->to_string() << std::endl ;
	std::cout << "\n" << a->to_string() << std::endl ;
}
