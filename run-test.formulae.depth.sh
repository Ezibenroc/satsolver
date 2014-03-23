#!/bin/bash

if [ $# != 1 ]
then
echo "Please provide the number of tests to perform."
exit 1
fi

NB_TEST=$1								# number of tests to perform for each configuration
DIRECTORY=/tmp						# working directory where files will be saved
EXEC="./tseitin"					# executable
NB_VAR=10

# preparation du fichier comparaison.dat: on l'enleve s'il existe, 
rm -f comparaison.dat
rm -f $DIRECTORY/output.txt
echo "Depth Time" >> comparaison.dat

for depth in `seq 1 7`; do
	TIME=0

  echo "Computing test for" $depth "depth."

	# On fait plusieurs tests par taille
	for test in `seq 1 $NB_TEST` ; do
		# Génération de la formule dans le fichier $DIRECTORY/formula.cnf
		./generator -nvar $NB_VAR -depth $depth -o $DIRECTORY/formula.cnf

		# Résolution de la formule 
	
		# Heuristique DUMB
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC $DIRECTORY/formula.cnf > $DIRECTORY/output.txt
		TMP=`cat $DIRECTORY/result.txt`
		TIME=$(echo "scale=3; $TIME + $TMP" | bc)
		
	done
	TIME=$(echo "scale=3; $TIME / $NB_TEST" | bc)
	echo $depth $TIME >> ./comparaison.dat 
# fin de la boucle
done

# gnuplot script-plot.p

# evince courbe1.pdf &

# Si une formule n'était pas satisfaite par l'affectation, on aura des "assert" dans le fichier
#grep "assert" $DIRECTORY/output.txt
