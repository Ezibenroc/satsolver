#ifndef STRUCTURES_FORMULA_H
#define STRUCTURES_FORMULA_H

#include <vector>
#include <string>
#include <memory>
#include <stack>

#include "structures/clause.h"

namespace satsolver {

class Formula {
    private :
        Affectation *aff ;
        std::vector<std::shared_ptr<satsolver::Clause>> clauses ;
        int nb_variables ;
        std::stack<std::pair<int,bool>> mem ; // littéraux dans l'ordre où on les affecte (vrai si déduit, faux si parié)
        std::set<int> to_do ; // littéraux déduits pas encore affectés (d’autres opérations sont « en cours »)

        // Affectation d'un litéral x
        // Si WITH_WL, renvoie faux ssi un conflit est généré
        bool set_true(int x, unsigned int *clause_id) ;


    public :
        Formula(std::vector<std::shared_ptr<satsolver::Clause>> v, int nb_variables) ;
        Formula(const Formula&);
        Formula& operator=(const Formula &that);
        Formula(satsolver::Formula *f) ;
        ~Formula() ;


        // Renvoie la représentation textuelle de la formule
        std::string to_string() ;
        std::string to_string2() ;

        // Renvoie un ensemble d'ensembles d'entiers, selon l'affectation
        // Chaque ensemble de cet ensemble représente une clause
        // Une clause vraie ne sera pas mise dans l'ensemble
        // Une clause fausse sera un ensemble vide
        // Les clauses ne contiendront que des littéraux indeterminés
        std::set<std::set<int> > to_set() ;
        std::set<Clause*> to_clauses_set() const;
        std::vector<std::shared_ptr<Clause>>& to_clauses_vector();

        // Déduction de l'affectation d'un littéral
        // Si WITH_WL, renvoie faux ssi conflit
        bool deduce_true(int x, unsigned int *clause_id) ;
        bool deduce_false(int x, unsigned int *clause_id) ;

        // Pari sur l'affectation d'un littéral
        // Si WITH_WL, renvoie faux ssi conflit
        bool bet_true(int x, unsigned int *clause_id) ;
        bool bet_false(int x, unsigned int *clause_id) ;

        // Retourne en arrière jusqu'au dernier pari
        // Renvoie le dernier littéral parié (0 si inexistant)
        int back() ;

        // Renvoie un monome de la formule (0 si inexistant)
        int monome(unsigned int *clause_id) ;

        // Renvoie un litéral isolé de la formule (0 si inexistant)
        int isolated_literal(unsigned int *clause_id) ;

        // Renvoie vrai ssi la formule ne contient pas de clauses
        bool is_empty() const;

        // Renvoie le nombre de variables
        int get_nb_variables() const;

        // Renvoie la taille de la formule ;
        long unsigned int get_size() const ;

        // Renvoie vrai ssi la formule contient une clause vide
        bool contains_empty_clause() const;

        // Determines whether one of the clauses is evaluated to false.
        bool contains_false_clause(unsigned int *clause_id) const;
        
        // Determines whether all the clauses are evaluated to true.
        bool only_true_clauses(unsigned int *clause_id) const;

        // Supprime toute les clauses contenant d'autres clauses
        // Affecte tous les monomes
        // Assez lourd (nombre de clauses au carré fois le nombre de variables)
        // À utiliser seulement à l'initialisation
        void clean() ;

        // Renvoie la pile d'affectations
        // Ne pas utiliser en dehors des tests unitaires
        std::stack<std::pair<int,bool>> get_mem() ;

        // Renvoie l'affectation
        // Ne pas utiliser en dehors des tests unitaires, ou de la fin de DPLL
        Affectation *get_aff() ;
        
        // Renvoie un vecteur de littéraux inconnus
        std::vector<int> get_unknown_literals (void) const ;

        // Renvoie un littéral de la formule
        // Pré-condition : la formule n'est pas vide, et n'est pas le monome clause vide
        int choose_literal_dumb() const;
        
        // Choisis un littéral aléatoirement dans la formule
        // Pré-condition : le générateur pseudo-aléatoire a été initialisé correctement
        int choose_literal_random() const ;
        
        // Choisis un littéral avec l'heuristique MOMS
        int choose_literal_moms() const ;
     
        // Choisis un littéral avec l'heuristique DLIS (variante)
        int choose_literal_dlis() const ;
        
        // Choisis un littéral selon l'heuristique spécifiée
        int choose_literal(int choice) ;
};

}

#endif
