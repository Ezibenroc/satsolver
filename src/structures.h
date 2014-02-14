#include <vector>
#include <string>
#include <set>

namespace satsolver {

class Affectation {
	private : 
			std::vector<int> aff ; // aff[i-1] = 1 si i est vrai, -1 si i est faux, 0 si i est indéterminé
			int nb_aff ; // nombre de variables affectées
	
	public :
			Affectation(int nb_variables) ; // nb_variables est le nombre de variables dans le problème
			
			// Connaître l'état d'une variable
			bool is_true(int x) ;
			bool is_false(int x) ;
			bool is_unknown(int x) ;
			
			// Changer l'état d'une variable
			void set_true(int x) ;
			void set_false(int x) ;
			void set_unknown(int x) ;
};

class Clause {
    private:
        int size; 				// nombre de literaux dans la clause
        bool *literals;		// clause 
        int nb_variables;	// nombre de variables dans le problème	
    
        int nb_unknown ; 	// nombre de littéraux non affectés
        int nb_true ;			// nombre de littéraux affectés à vrai
        int nb_false ; 		// nombre de littéraux affectés à faux 			

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
        // Non utilisé dans l'algorithme.
       	// void remove(int literal) ;
        
        // Renvoie la représentation textuelle de la clause
        std::string to_string() ;

        // Renvoie un ensemble contenant tous les entiers de la clause. 
        std::set<int> to_set() ;
       
       // A PRIORI INUTILE : pas d'union dans DPLL 
       // Clause* disjonction(Clause *clause2);
       
       
       // Incrémente nb_true, décréente nb_unknown
       void incr_true() ;
       // Incrémente nb_false, décréente nb_unknown
       void incr_false() ;
       // Décrémente nb_true, incrémente nb_unknown
       void decr_true() ;
       // Décrémente nb_true, incrémente nb_unknown
       void decr_false() ;
       // Renvoie vrai si la clause est vraie, i.e. si nb_true > 0
       bool is_true() ;
       // Renvoie vrai si la clause est fausse, i.e. si nb_false = size
       bool is_false() ;
       // Si tous les littéraux de la clause sont faux, sauf un indéterminé, renvoie son indice
       // Sinon renvoie 0
       int monome(Affectation a) ; 
};


}
