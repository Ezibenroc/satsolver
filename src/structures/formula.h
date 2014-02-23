#ifndef STRUCTURES_FORMULA_H
#define STRUCTURES_FORMULA_H

#include <vector>
#include <string>
#include <memory>

#include "clause.h"

namespace satsolver {

class Formula {
    private :
        std::vector<std::shared_ptr<satsolver::Clause>> clauses ;
        int nb_variables ;


    public :
        Formula(std::vector<std::shared_ptr<satsolver::Clause>> v, int nb_variables) ;
        Formula& operator=(const Formula &that);
        Formula(satsolver::Formula *f) ;
        ~Formula() ;

        // Renvoie le premier entier x tel qu'il existe une clause monome {x} (0 s'il n'existe pas)
        int find_monome() const;

        /* Propagation unitaire
        Trouve une clause monome, et affecte à vrai son littéral.
        Si succés, renvoie le literal affecté à vrai, sinon renvoie 0. */
        //	int unit_propagation() ;

        // Renvoie le premier entier x tel que x soit présent dans au moins une clause, et -x absent de toute clauses
        // (0 s'il n'existe pas)
        int find_isolated_literal() const;

        // Renvoie la représentation textuelle de la formule
        std::string to_string() const;

        // Renvoie un ensemble d'ensembles d'entiers, selon l'affectation
        // Chaque ensemble de cet ensemble représente une clause
        // Une clause vraie ne sera pas mise dans l'ensemble
        // Une clause fausse sera un ensemble vide
        // Les clauses ne contiendront que des littéraux indeterminés
        std::set<std::set<int> > to_set() ;
        std::set<Clause*> to_clauses_set() ;

        // Affectation d'un litéral x
        void set_true(int x) ;
        void set_false(int x) ;


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
        // Assez lourd (nombre de clauses au carré fois le nombre de variables)
        // À utiliser avec parcimonie
        void clean() ;
};

}

#endif
