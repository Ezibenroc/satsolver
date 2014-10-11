# SatSolver

The present file is a partial translation of the original README, written in french.

## Utilization

### Compilation

Depends: `cmake` (optional: `libcppunit`).
``` 
mkdir build/
cd build/
cmake ..
make
```

### Execution

The compilation produces several programs: 

* `resol` the main program, find a truth affectation of a set of clauses if it 
exists, print some message otherwise.
* `resol_diff` an adaptation of the main program to solve SMT problem, where the
considered theory is the difference logic.
* `resol_equal` an other adaptation of the main program to solve SMT problem, where 
the considered theory is the empty theory.
* `colorie` a solver for the graph coloration problem, using the satsolver.
* `tseitin` a solver of propositional logic formulae, using the satsolver.

All can take the following parameters, which have to be placed before anything else.

* `-verbose` activate verbosity mode
* `-WL` activate Watched Literals heuristic
* `-rand`, `-moms` or `-dlis` (only one among the three), activate the corresponding
heuristic.
* `-CL` activate the Clause Learning.
* `-cl-interac` activate the interactive mode.

The program `colorie` also take an integer, which is the number of colors to use.

The program `tseitin` can take two additional parameters: `-print-interpretation`
which print the parsed formula, and `-print-sat` which print the set of clauses
obtained by the Tseitin transformation.

All these programs can take a file name as last parameter. The corresponding file
will then be used as input. If no name is given, then the standard input is used.

Input files for `resol` and `colorie` are respectively in format `cnf` and `edge`
of the `DIMACS`. Input files of `tseitin`, `resol_diff` and `resol_equal` are 
non-standard format. See the given examples.
