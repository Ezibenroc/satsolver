#ifndef FORMULA_WITH_WL_H
#define FORMULA_WITH_WL_H
#include "formula.h"

namespace satsolver {

class FormulaWithWL : public Formula {
    private:
        Affectation *aff ;
        std::stack<std::pair<int,bool>> mem ; // littéraux dans l'ordre où on les affecte (vrai si déduit, faux si parié)
        std::set<int> to_do ; // littéraux déduit pas encore affectés

        // Affectation d'un litéral x
        // Renvoie faux si un conflit est généré (vrai sinon)
        bool set_true(int x) ;

    public:
        FormulaWithWL(satsolver::Formula *f);
        FormulaWithWL& operator=(const FormulaWithWL &that);
        FormulaWithWL(satsolver::FormulaWithWL *f) ;
        ~FormulaWithWL() ;

        // Déduction de l'affectation d'un littéral
        // Renvoie faux ssi conflit
        bool deduce_true(int x) ;
        bool deduce_false(int x) ;
        
        // Pari sur l'affectation d'un littéral
        // Renvoie faux ssi conflit
        bool bet_true(int x) ;
        bool bet_false(int x) ;

        // Retourne en arrière jusqu'au dernier paris
        // Renvoie le dernier littéral parié (0 si inexistant) 
        int back() ;

        // Renvoie un litéral isolé de la formule (0 si inexistant)
        int isolated_literal() ;

        // Supprime toute les clauses contenant d'autres clauses
        // Affecte tous les monomes
        // Assez lourd (nombre de clauses au carré fois le nombre de variables)
        // À utiliser seulement à l'initialisation
        virtual void clean() ;
        
        // Renvoie la pile d'affectations
        // Ne pas utiliser en dehors des tests unitaires
        std::stack<std::pair<int,bool>> get_mem() ;
    		
        // Renvoie l'affectation
        // Ne pas utiliser en dehors des tests unitaires, ou de la fin de DPLL
        Affectation *get_aff() ;		    

        // Renvoie un littéral de la formule
        // Pré-condition : la formule n'est pas vide, et n'est pas le monome clause vide
        virtual int choose_literal() const;

        virtual std::set<std::set<int> > to_set() const;
};

}

#endif
