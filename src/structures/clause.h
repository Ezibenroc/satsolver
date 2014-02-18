#ifndef STRUCTURES_CLAUSE_H
#define STRUCTURES_CLAUSE_H

#include <set>
#include <string>
#include <vector>

#include "affectation.h"

namespace satsolver {

class Clause {
		private:
        int size; 				// nombre de literaux dans la clause
        bool *literals;		// clause 
        int nb_variables;	// nombre de variables dans le problème	
    
        int nb_unknown ; 	// nombre de littéraux non affectés
        int nb_true ;			// nombre de littéraux affectés à vrai
        int nb_false ; 		// nombre de littéraux affectés à faux 
        
        
        void decr_true() ;
        void incr_true() ;
        void decr_false() ;
        void incr_false() ;			

    public:   
    
        Clause(int nb_variables, bool *literals);
        Clause(int nb_variables, std::vector<int> literals);
        Clause(const satsolver::Clause &c) ; // initialise une nouvelle clause, copie de la clause donnée
        ~Clause();
				
				// Test si le litéral donné appartient à la clause
        bool contains_literal(int literal);
        
        // Ajoute le litéral donné à la clause
        void add(int literal) ;
        
        // Test si la clause est une tautologie
        bool is_tautology() ;
        
        // Renvoie le nombre de littéraux
        int get_size() ;
        
        // Supprime le litéral donné de la clause
        // Non utilisé dans l'algorithme.
       	// void remove(int literal) ;
        
        // Renvoie la représentation textuelle de la clause
        std::string to_string() ;

        // Renvoie un ensemble contenant tous les entiers de la clause. 
        std::set<int> to_set() ;
       
       // A PRIORI INUTILE : pas d'union dans DPLL 
       // Clause* disjonction(Clause *clause2);
       
       // Modifie nb_false et nb_unknown, en supposant que x était avant indéterminé
       void set_false(int x) ;
       // Modifie nb_true et nb_unknown, en supposant que x était avant indéterminé
       void set_true(int x) ;
       // Modifie nb_false et nb_unknown, en supposant que x était avant faux
       void unset_false(int x) ;
       // Modifie nb_true et nb_unknown, en supposant que x était avant vrai
       void unset_true(int x) ;
       
       bool is_true() ;
       // Renvoie vrai si la clause est fausse, i.e. si nb_false = size
       bool is_false() ;
       // Si tous les littéraux de la clause sont faux, sauf un indéterminé, renvoie son indice
       // Sinon renvoie 0
       int monome(Affectation a) ; 
};

}

#endif
