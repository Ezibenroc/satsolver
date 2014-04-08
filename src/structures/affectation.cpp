#include <cstdlib>
#include <cassert>
#include <sstream>
#include <iostream>

#include "affectation.h"

#define abs(x) ((static_cast<unsigned int>(x > 0 ? x : -x)))

using namespace satsolver;

Affectation::Affectation(int nb_var) : aff(std::vector<int>()), nb_aff(0), nb_unknown(nb_var) {
    for(int i = 0 ; i < nb_var ; i++)
        this->aff.push_back(0) ;
}

Affectation::Affectation(Affectation *a) : aff(std::vector<int>(a->aff)), nb_aff(a->nb_aff), nb_unknown(a->nb_unknown) {
}

bool Affectation::is_true(int x) const {
    assert(abs(x) <= this->aff.size() && x!=0) ;
    if(x>0)
        return this->aff[x-1] == TR ;
    else
        return this->is_false(-x);
}
bool Affectation::is_false(int x) const {
    assert(abs(x) <= this->aff.size() && x!=0) ;
    if(x>0)
        return this->aff[x-1] == FA ;
    else
        return this->is_true(-x);
}
bool Affectation::is_unknown(int x) const {
    assert(abs(x) <= this->aff.size() && x!=0) ;
    if (x>0)
        return this->aff[x-1] == UN ;
    else
        return this->is_unknown(-x);
}

void Affectation::set_true(int x) {
    if(x>0) {
        assert(abs(x) <= this->aff.size() && x!=0) ;
        assert(this->is_unknown(x)) ;
        if(is_unknown(x))
            this->nb_unknown -- ;
        this->aff[x-1] = TR ;
    }
    else
        this->set_false(-x);
}
void Affectation::set_false(int x) {
    if(x>0) {
        assert(abs(x) <= this->aff.size() && x!=0) ;
        assert(this->is_unknown(x)) ;
        if(is_unknown(x))
            this->nb_unknown -- ;
        this->aff[x-1] = FA ;
    }
    else
        this->set_true(-x);
}
void Affectation::set_unknown(int x) {
    if(x>0) {
        assert(abs(x) <= this->aff.size() && x!=0) ;
        if(!is_unknown(x))
            this->nb_unknown ++ ;
        this->aff[abs(x)-1] = UN ;
    }
    else
        this->set_unknown(-x);
}

std::string Affectation::to_string() const {
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

std::set<int>* Affectation::to_set() const {
    std::set<int> *set = new std::set<int>();
    for(unsigned int i = 1 ; i <= this->aff.size() ; i++) {
        if(this->is_true(i))
            set->insert(i);
        else if(this->is_false(i))
            set->insert(-i);
    }
    return set;
}

int Affectation::get_nb_unknown() {
	return this->nb_unknown ;
}

