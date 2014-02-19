#ifndef STRUCTURES_AFFECTATION_H
#define STRUCTURES_AFFECTATION_H

#include <set>
#include <string>
#include <vector>

namespace satsolver {

class Affectation {
	private : 
			std::vector<int> aff ; // aff[i-1] = 1 si i est vrai, -1 si i est faux, 0 si i est indéterminé
			int nb_aff ; // nombre de variables affectées
	
	public :
			Affectation(int nb_variables) ; // nb_variables est le nombre de variables dans le problème
			
			// Connaître l'état d'une variable
			bool is_true(int x) const;
			bool is_false(int x) const;
			bool is_unknown(int x) const;
			
			// Changer l'état d'une variable
			void set_true(int x) ;
			void set_false(int x) ;
			void set_unknown(int x) ;

            std::string to_string() const;

            std::set<int>* to_set() const;
};

}

#endif
