#ifndef STRUCTURES_FORMULA_H
#define STRUCTURES_FORMULA_H

#include <vector>
#include <string>

#include "clause.h"

namespace satsolver {

class Formula {
		private :
			std::vector<satsolver::Clause*> clauses ;
			int nb_variables ;
			
			
		public :
			Formula(std::vector<satsolver::Clause*> v, int nb_variables) ;
			Formula(const satsolver::Formula *f) ;
			~Formula() ;			

			// Renvoie le premier entier x tel qu'il existe une clause monome {x} (0 s'il n'existe pas)
			int find_monome() const;
			
        // Renvoie la représentation textuelle de la formule
        std::string to_string() const;
			
      // Renvoie un ensemble d'ensembles d'entiers, selon l'affectation
      // Chaque ensemble de cet ensemble représente une clause
      // Une clause vraie ne sera pas mise dans l'ensemble
      // Une clause fausse sera un ensemble vide
      // Les clauses ne contiendront que des littéraux indeterminés
      std::set<std::set<int> > to_set() ;
			
			// Affectation d'un litéral x
			void set_true(int x) ;
			void set_false(int x) ;
			
			/* Propagation unitaire
			Trouve une clause monome, et affecte à vrai son littéral.
			Si succés, renvoie vrai, sinon renvoie faux. */
			bool unit_propagation() ;

            bool is_empty() const;
            int get_nb_variables() const;
            bool contains_empty_clause() const;
            int choose_literal() const; 
};

}

#endif
