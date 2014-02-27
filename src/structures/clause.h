#ifndef STRUCTURES_CLAUSE_H
#define STRUCTURES_CLAUSE_H

#include <set>
#include <string>
#include <vector>
#include "affectation.h"

namespace satsolver {

class Conflict: public std::exception {};

class Clause {
        private:
        std::set<int> literals ;
        int nb_variables;    // nombre de variables dans le problème
        std::pair<int,int> watched ;
        Affectation *aff ; // pointeur vers l'affectation globale

    public:

        Clause(int nb_variables, std::vector<int> literals);
        Clause(const satsolver::Clause &c) ; // initialise une nouvelle clause, copie de la clause donnée
        Clause& operator=(const Clause &that);
        ~Clause();

        // Teste si le litéral donné appartient à la clause
        bool contains_literal(int literal) const;

        // Ajoute le litéral donné à la clause
        void add(int literal) ;

        // Supprime le litéral donné de la clause
        // Renvoie l'exception Conflict si la clause est vide après la suppression
        void remove(int literal) ;

        // Teste si la clause est une tautologie
        bool is_tautology() const;

        // Renvoie le nombre de littéraux
        int get_size() const;



        // Renvoie la représentation textuelle de la clause
        std::string to_string() ;

        // Renvoie un ensemble contenant tous les entiers de la clause.
        std::set<int> to_set() const;


       // Renvoie vrai si la clause est vide
       bool is_empty() const;

       // Renvoie vrai ssi elle contient la clause donnée
       bool contains_clause(satsolver::Clause &c) const;

       // Initialise les watched literals
       void init_WL() ;

       // Renvoie le premier ou le second watched literal
       int fst_WL() ;
       int snd_WL() ;

       // Test si l'entier donné est un watched literal
       bool is_WL(int x) ;

       // Définie l'affectation
       void set_affectation(Affectation *a) ;

       // Annonce à la clause que x a été mis à vrai
       // Retourne y si y devient le seul littéral non faux, 0 sinon
       int set_true(int x) ;
       int set_false(int x) ;

       bool is_true() ;

       // Si la clause est un monome {x}, renvoie x (appelé seulement dans clean)
       int monome_begin() ;

       // Si tous les litteraux sont faux, sauf un qui est indéterminé, renvoie ce littéral
       int monome() ;

       // Determines whether all literals in the clause are set to false.
       bool is_evaluated_to_false() const;
};

}

#endif
