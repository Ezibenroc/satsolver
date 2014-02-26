#ifndef STRUCTURES_FORMULA_H
#define STRUCTURES_FORMULA_H

#include <vector>
#include <string>
#include <memory>
#include <stack>

#include "clause.h"

namespace satsolver {

class Formula {
    private :
        std::vector<std::shared_ptr<satsolver::Clause>> clauses ;
        int nb_variables ;
        Affectation *aff ;
        std::stack<std::pair<int,bool>> mem ; // littéraux dans l'ordre où on les affecte (vrai si déduit, faux si parié)
        std::set<int> to_do ; // littéraux déduit pas encore affectés

        // Affectation d'un litéral x
        // Exception Conflict si un conflit est généré
        void set_true(int x) ;


    public :
        Formula(std::vector<std::shared_ptr<satsolver::Clause>> v, int nb_variables) ;
        Formula& operator=(const Formula &that);
        Formula(satsolver::Formula *f) ;
        ~Formula() ;


        // Renvoie la représentation textuelle de la formule
        std::string to_string() const;

        // Renvoie un ensemble d'ensembles d'entiers, selon l'affectation
        // Chaque ensemble de cet ensemble représente une clause
        // Une clause vraie ne sera pas mise dans l'ensemble
        // Une clause fausse sera un ensemble vide
        // Les clauses ne contiendront que des littéraux indeterminés
        std::set<std::set<int> > to_set() ;
        std::set<Clause*> to_clauses_set() ;

				// Déduction de l'affectation d'un littéral
				void deduce_true(int x) ;
				void deduce_false(int x) ;
				
				// Pari sur l'affectation d'un littéral
				void bet_true(int x) ;
				void bet_false(int x) ;

				// Retourne en arrière jusqu'au dernier paris
				// Renvoie le dernier littéral parié (0 si inexistant) 
				int back() ;

				// Renvoie un litéral isolé de la formule (0 si inexistant)
				int isolated_literal() ;

        // Renvoie vrai ssi la formule ne contient pas de clauses
        bool is_empty() const;

        // Renvoie le nombre de variables
        int get_nb_variables() const;

        // Renvoie la taille de la formule ;
        int get_size() const ;

        // Renvoie vrai ssi la formule contient une clause vide
        bool contains_empty_clause() const;

        // Renvoie un littéral de la formule
        // Pré-condition : la formule n'est pas vide, et n'est pas le monome clause vide
        int choose_literal() const;

        // Supprime toute les clauses contenant d'autres clauses
        // Affecte tous les monomes
        // Assez lourd (nombre de clauses au carré fois le nombre de variables)
        // À utiliser seulement à l'initialisation
        void clean() ;
        
        // Renvoie la pile d'affectations
        // Ne pas utiliser en dehors des tests unitaires
        std::stack<std::pair<int,bool>> get_mem() ;
    		
    		// Renvoie l'affectation
    		// Ne pas utiliser en dehors des tests unitaires
    		Affectation *get_aff() ;		    
};	

}

#endif
