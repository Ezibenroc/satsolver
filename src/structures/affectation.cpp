#include <cstdlib>
#include <cassert>
#include <sstream>
#include <iostream>

#include "affectation.h"

using namespace satsolver;

Affectation::Affectation(int nb_var) {
	std::vector<int> t ;
	this->nb_aff = 0 ;
	this->aff = t ;
	for(int i = 0 ; i < nb_var ; i++)
		this->aff.push_back(0) ;
}

bool Affectation::is_true(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
		return this->aff[x-1] == 1 ;
	else
        return this->is_false(-x);
}
bool Affectation::is_false(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
		return this->aff[x-1] == -1 ;
	else
        return this->is_true(-x);
}
bool Affectation::is_unknown(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
    if (x>0)
        return this->aff[x-1] == 0 ;
    else
        return this->is_unknown(-x);
}

void Affectation::set_true(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
		this->aff[x-1] = 1 ;
	else
        this->set_false(-x);
}
void Affectation::set_false(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
		this->aff[x-1] = -1 ;
	else
        this->set_true(-x);
}
void Affectation::set_unknown(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
        this->aff[abs(x)-1] = 0 ;
    else
        this->set_unknown(-x);
}

std::string Affectation::to_string() {
	std::ostringstream oss;
	oss << "{" ;
	for(unsigned int i = 1 ; i <= this->aff.size() ; i++) {
        if (i>1)
            oss << ", ";
		if(this->is_true(i))
			oss << i << "=" << "true";
        else if(this->is_false(i))
			oss << i << "=" << "false";
        else
			oss << i << "=" << "unknown";
	}
	oss << "}" ;
	return oss.str() ;
}
