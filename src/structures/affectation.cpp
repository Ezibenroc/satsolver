#include <cstdlib>
#include <cassert>

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
		return this->aff[abs(x)-1] == -1 ;
}
bool Affectation::is_false(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
		return this->aff[x-1] == -1 ;
	else
		return this->aff[abs(x)-1] == 1 ;
}
bool Affectation::is_unknown(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	return this->aff[abs(x)-1] == 0 ;
}

void Affectation::set_true(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
		this->aff[x-1] = 1 ;
	else
		this->aff[abs(x)-1] = -1 ;
}
void Affectation::set_false(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	if(x>0)
		this->aff[x-1] = -1 ;
	else
		this->aff[abs(x)-1] = 1 ;
}
void Affectation::set_unknown(int x) {
	assert(abs(x) <= (int) this->aff.size() && x!=0) ;
	this->aff[abs(x)-1] = 0 ;
}
