#ifndef STRUCTURES_CLAUSE_H
#define STRUCTURES_CLAUSE_H

#include <set>
#include <string>
#include <vector>

namespace satsolver {

class Conflict: public std::exception {};

class Clause {
		private:
        int size; 				// nombre de literaux dans la clause
        bool *literals;		// clause 
        int nb_variables;	// nombre de variables dans le problème	

    public:   
    
        Clause(int nb_variables, bool *literals);
        Clause(int nb_variables, std::vector<int> literals);
        Clause(const satsolver::Clause &c) ; // initialise une nouvelle clause, copie de la clause donnée
        ~Clause();
				
				// Test si le litéral donné appartient à la clause
        bool contains_literal(int literal);
        
        // Ajoute le litéral donné à la clause
        void add(int literal) ;
        
        // Supprime le litéral donné de la clause
        // Renvoie l'exception Conflict si la clause est vide après la suppression
        void remove(int literal) ;
        
        // Test si la clause est une tautologie
        bool is_tautology() ;
        
        // Renvoie le nombre de littéraux
        int get_size() ;
        

        
        // Renvoie la représentation textuelle de la clause
        std::string to_string() ;

        // Renvoie un ensemble contenant tous les entiers de la clause. 
        std::set<int> to_set() ;
       

       // Renvoie vrai si la clause est vide
       bool is_empty() ;
       // Si la clause est un monome, renvoie le literal en question
       // Sinon renvoie 0
       int monome() ; 
       
       // Renvoie vrai ssi elle contient la clause donnée
       bool contains_clause(satsolver::Clause &c) ;
};

}

#endif
