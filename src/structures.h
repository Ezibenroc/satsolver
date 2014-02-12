#include <vector>
#include <string>
#include <set>

namespace satsolver {

class Clause {
    private:
        int m_size; 				// nombre de literaux dans la clause
        bool *m_literals;		// clause 
        int m_nb_variables;	// nombre de variables dans le problème				

    public:
        Clause(int nb_variables, bool *literals);
        Clause(int nb_variables, std::vector<int> literals);
        Clause(const satsolver::Clause &c) ; // initialise une nouvelle clause, copie de la clause donnée
				
				// Test si le litéral donné appartient à la clause
        bool contains_literal(int literal);
        
        // Ajoute le litéral donné à la clause
        void add(int literal) ;
        
        // Supprime le litéral donné de la clause
        void remove(int literal) ;
        
        // Renvoie la représentation textuelle de la clause
        std::string to_string() ;

        // Renvoie un ensemble contenant tous les entiers de la clause. 
        std::set<int> to_set() ;
       
       // A PRIORI INUTILE : pas d'union dans DPLL 
       // Clause* disjonction(Clause *clause2);
};

}
