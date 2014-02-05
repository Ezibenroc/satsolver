#include <vector>

namespace satsolver {

class Clause {
    private:
        int m_size; 				// nombre de literaux dans la clause
        bool *m_literals;		// clause 
        int m_nb_variables;	// nombre de variables dans le problème				

    public:
        Clause(int nb_variables, bool *literals);
        Clause(int nb_variables, std::vector<int> literals);
				
				// Test si le litéral donné appartient à la clause
        bool contains_literal(int literal);
        
        // Aoute le litéral donné à la clause
        void add(int literal) ;
        
        // Supprime le litéral donné de la clause
        void remove(int literal) ;
        Clause* disjonction(Clause *clause2);
};

}
