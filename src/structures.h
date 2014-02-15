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
        
        
        void decr_true() ;
        void incr_true() ;
        void decr_false() ;
        void incr_false() ;			

    public:   
    
        Clause(int nb_variables, bool *literals);
        Clause(int nb_variables, std::vector<int> literals);
        Clause(const satsolver::Clause &c) ; // initialise une nouvelle clause, copie de la clause donnée
        ~Clause();
				
				// Test si le litéral donné appartient à la clause
        bool contains_literal(int literal);
        
        // Ajoute le litéral donné à la clause
        void add(int literal) ;
        
        // Test si la clause est une tautologie
        bool is_tautology() ;
        
        // Renvoie le nombre de littéraux
        int get_size() ;
        
        // Supprime le litéral donné de la clause
        // Non utilisé dans l'algorithme.
       	// void remove(int literal) ;
        
        // Renvoie la représentation textuelle de la clause
        std::string to_string() ;

        // Renvoie un ensemble contenant tous les entiers de la clause. 
        std::set<int> to_set() ;
       
       // A PRIORI INUTILE : pas d'union dans DPLL 
       // Clause* disjonction(Clause *clause2);
       
       // Modifie nb_false et nb_unknown, en supposant que x était avant indéterminé
       void set_false(int x) ;
       // Modifie nb_true et nb_unknown, en supposant que x était avant indéterminé
       void set_true(int x) ;
       // Modifie nb_false et nb_unknown, en supposant que x était avant faux
       void unset_false(int x) ;
       // Modifie nb_true et nb_unknown, en supposant que x était avant vrai
       void unset_true(int x) ;
       
       bool is_true() ;
       // Renvoie vrai si la clause est fausse, i.e. si nb_false = size
       bool is_false() ;
       // Si tous les littéraux de la clause sont faux, sauf un indéterminé, renvoie son indice
       // Sinon renvoie 0
       int monome(Affectation a) ; 
};


class Formula {
		private :
			std::vector<satsolver::Clause> clauses ;
			satsolver::Affectation aff ;
			int nb_variables ;
			
			// Renvoie le premier entier x tel qu'il existe une clause monome {x} (0 s'il n'existe pas)
			int find_monome() ;
			
		public :
			Formula(std::vector<satsolver::Clause> v, int nb_variables) ;
			~Formula() ;			
			
      // Renvoie un ensemble d'ensembles d'entiers, selon l'affectation
      // Chaque ensemble de cet ensemble représente une clause
      // Une clause vraie ne sera pas mise dans l'ensemble
      // Une clause fausse sera un ensemble vide
      // Les clauses ne contiendront que des littéraux indeterminés
      std::set<std::set<int>> to_set() ;
			
			/* Propagation unitaire
			Trouve une clause monome, et affecte à vrai son littéral.
			Si succés, renvoie vrai, sinon renvoie faux. */
			bool unit_propagation() ;
};

}
