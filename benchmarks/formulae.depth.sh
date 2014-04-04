#!/bin/bash

if [ $# != 1 ]
then
echo "Please provide the number of tests to perform."
exit 1
fi

NB_TEST=$1								# nombre de tests pour chaque configuration
DIRECTORY=/tmp						# endroit de sauvegarde des fichiers temporaires
EXEC="./tseitin"					# executable
NVAR=5
OUTPUT=formulae.depth.dat

rm -f $OUTPUT
rm -f /dev/null
echo "Depth DUMB RAND MOMS DLIS DUMB_WL RAND_WL MOMS_WL DLIS_WL" >> $OUTPUT

for depth in `seq 1 20`; do
	TIME_DUMB=0
	TIME_RAND=0
	TIME_MOMS=0
	TIME_DLIS=0
	
	TIME_DUMB_WL=0
	TIME_RAND_WL=0
	TIME_MOMS_WL=0
	TIME_DLIS_WL=0	
	
  echo "Computing test for " $depth " depth."

	# On fait plusieurs tests par taille

	for test in `seq 1 $NB_TEST` ; do
		echo -e "\t\tTest $test"
		# Génération de la formule dans le fichier $DIRECTORY/formula.cnf
		./generator -nvar $NVAR -depth $depth -o $DIRECTORY/formula.cnf

		# Résolution de la formule
	
		# Heuristique DUMB
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB=$(echo "scale=3; $TIME_DUMB + $TMP" | bc)
		# Heuristique RAND
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -rand $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND=$(echo "scale=3; $TIME_RAND + $TMP" | bc)
		# Heuristique MOMS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -moms $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS=$(echo "scale=3; $TIME_MOMS + $TMP" | bc)
		# Heuristique DLIS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -dlis $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS=$(echo "scale=3; $TIME_DLIS + $TMP" | bc)
		
		# La même chose, avec les watched literals
		
				# Heuristique DUMB
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB_WL=$(echo "scale=3; $TIME_DUMB_WL + $TMP" | bc)
		# Heuristique RAND
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -rand $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND_WL=$(echo "scale=3; $TIME_RAND_WL + $TMP" | bc)
		# Heuristique MOMS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -moms $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS_WL=$(echo "scale=3; $TIME_MOMS_WL + $TMP" | bc)
		# Heuristique DLIS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -dlis $DIRECTORY/formula.cnf > /dev/null
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS_WL=$(echo "scale=3; $TIME_DLIS_WL + $TMP" | bc)
		
	done
	TIME_DUMB=$(echo "scale=3; $TIME_DUMB / $NB_TEST" | bc)
	TIME_RAND=$(echo "scale=3; $TIME_RAND / $NB_TEST" | bc)	
	TIME_MOMS=$(echo "scale=3; $TIME_MOMS / $NB_TEST" | bc)	
	TIME_DLIS=$(echo "scale=3; $TIME_DLIS / $NB_TEST" | bc)	
	
	
	TIME_DUMB_WL=$(echo "scale=3; $TIME_DUMB_WL / $NB_TEST" | bc)
	TIME_RAND_WL=$(echo "scale=3; $TIME_RAND_WL / $NB_TEST" | bc)	
	TIME_MOMS_WL=$(echo "scale=3; $TIME_MOMS_WL / $NB_TEST" | bc)	
	TIME_DLIS_WL=$(echo "scale=3; $TIME_DLIS_WL / $NB_TEST" | bc)	
	echo $depth $TIME_DUMB $TIME_RAND $TIME_MOMS $TIME_DLIS $TIME_DUMB_WL $TIME_RAND_WL $TIME_MOMS_WL $TIME_DLIS_WL >> $OUTPUT
# fin de la boucle
done

# gnuplot script-plot.p

# evince courbe1.pdf &

# Si une formule n'était pas satisfaite par l'affectation, on aura des "assert" dans le fichier
#grep "assert" /dev/null
