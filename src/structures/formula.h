#ifndef STRUCTURES_FORMULA_H
#define STRUCTURES_FORMULA_H

#include <vector>
#include <string>
#include <memory>
#include <stack>
#include <unordered_set>

#include "structures/clause.h"
#include "structures/affectation.h"
#include "structures/deductions.h"
#include "structures/CL_proof.h"

namespace satsolver {


// Deux foncteurs utilisés pour l'ensemble to_do.
struct Equal {
	bool operator() (const std::pair<int,unsigned int> &a, const std::pair<int,unsigned int> &b) const {
		  return (a.first == b.first);
	}
};
struct Hash {
	size_t operator() (const std::pair<int,unsigned int> &a) const {
		return a.first ;
	}
};

class Formula {
    private :
        Affectation *aff ;
        std::vector<std::shared_ptr<satsolver::Clause>> clauses ;
        int nb_variables ;
        std::vector<std::pair<int,bool>> mem ; // littéraux dans l'ordre où on les affecte (vrai si déduit, faux si parié)
        std::unordered_set<std::pair<int,unsigned int>,Hash,Equal> to_do ; // utilisé dans WL, littéraux déduits pas encore affectés (d’autres opérations sont « en cours »)
                              // le second entier est l'indice de la clause ayant permi de déduire le littéral
        Deductions *ded ;     // graphe orienté représentant les déductions
        unsigned int ded_depth ; // profondeur courante de l'arbre de déductions

        // Affectation d'un litéral x
        // Si WITH_WL, renvoie faux ssi un conflit est généré
        // Si WITH_WL et un conflit est généré, alors clause1 et clause2 sont mis à jour avec les indices des clauses en conflit (ainsi que literal)
        bool set_true(int x, int *clause1, int *clause2, int *literal) ;


    public :
        Formula(std::vector<std::shared_ptr<satsolver::Clause>> v, int nb_variables) ;
        Formula(const Formula&);
        Formula& operator=(const Formula &that);
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
        std::vector<std::shared_ptr<Clause>>& to_clauses_vector() ;

        void add_clause(std::shared_ptr<satsolver::Clause> clause);

        // Déduction de l'affectation d'un littéral
        // Si WITH_WL, renvoie faux ssi conflit
        // claude_id est l'indice de la clause qui est l'origine de cette déduction
        // pas de clause origine si clause_id = -1 (survient lors des déductions des littéraux isolés)
        bool deduce_true(int x, int clause_id, int *clause1, int *clause2, int *literal) ;
        bool deduce_false(int x, int clause_id) ;

        // Pari sur l'affectation d'un littéral
        // Si WITH_WL, renvoie faux ssi conflit
        bool bet_true(int x, int *clause1, int *clause2, int *literal) ;
        bool bet_false(int x, int *clause1, int *clause2, int *literal) ;

        // Retourne en arrière jusqu'au dernier pari
        // Renvoie le dernier littéral parié (0 si inexistant)
        int back() ;
        // Fait plusieurs backtrack, jusqu'à obtenir la profondeur de déduction donnée
        int back(unsigned int depth) ;

        // Renvoie un monome de la formule (0 si inexistant)
        int monome(int *clause_id) ;

        // Renvoie un litéral isolé de la formule (0 si inexistant)
        int isolated_literal(int *clause_id) ;

        // Renvoie vrai ssi la formule ne contient pas de clauses
        bool is_empty() const;

        // Renvoie le nombre de variables
        int get_nb_variables() const;

        // Renvoie la taille de la formule ;
        long unsigned int get_size() const ;

        // Renvoie vrai ssi la formule contient une clause vide
        bool contains_empty_clause() const;

        // Determines whether one of the clauses is evaluated to false.
        bool contains_false_clause(int *clause_id) const;
        
        // Determines whether all the clauses are evaluated to true.
        bool only_true_clauses(int *clause_id) const;

        // Supprime toute les clauses contenant d'autres clauses
        // Affecte tous les monomes
        // Assez lourd (nombre de clauses au carré fois le nombre de variables)
        // À utiliser seulement à l'initialisation
        void clean() ;

        // Renvoie la pile d'affectations
        // Ne pas utiliser en dehors des tests unitaires
        std::vector<std::pair<int,bool>> get_mem() ;

        // Renvoie l'affectation
        // Ne pas utiliser en dehors des tests unitaires, ou de la fin de DPLL
        Affectation *get_aff() ;
        
        // Renvoie les déductions
        Deductions *get_ded() ;
        // Renvoie la profondeur de déduction
        int get_ded_depth() ;
                
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
        
        
        // Apprentissage d'une clause
        // Le littéral donné doit être en conflis dans la clause d'indice donné
        // Clause_id est mis à jour avec l'indice de la clause apprise
        // New_depth est mis à jour avec la profondeur vers laquelle backtracker
        // Renvoie un littéral l que l'on doit déduire après le backtrack (sans déduire le dernier littéral backtracké)
        int learn_clause(CLProof *proof, int *clause_id, unsigned int *new_depth, int literal) ;
        
        // Renvoie le dernier paris
        int last_bet() ;
};

}

#endif
