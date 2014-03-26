#!/bin/bash

if [ $# != 1 ]
then
	echo "Please provide the number of tests to perform."
	exit 1
fi

NB_TEST=$1								# number of tests to perform for each configuration
DIRECTORY=/tmp						# working directory where files will be saved
EXEC="./colorie 4"					# executable
OUTPUT=graph.nvar.dat

rm -f $OUTPUT
rm -f $DIRECTORY/output.txt
echo "nb_nodes DUMB RAND MOMS DLIS" >> $OUTPUT

for nvar in `seq 5 20`; do
	TIME_DUMB=0
	TIME_RAND=0
	TIME_MOMS=0
	TIME_DLIS=0
  echo "Computing test for " $nvar " nodes."

	# On fait plusieurs tests par taille

	for test in `seq 1 $NB_TEST` ; do
		echo -e "\t\tTest $test"
		# Génération de la formule dans le fichier $DIRECTORY/graph
		./generator -nvar $nvar -nedge `expr 2 \* $nvar` -o $DIRECTORY/graph

		# Résolution de la formule
	
		# Heuristique DUMB
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC $DIRECTORY/graph > $DIRECTORY/output.txt 2> /tmp/error
		if [ -s /tmp/error ]
		then
			echo "EXIT after error DUMB"
			exit 1
		fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB=$(echo "scale=3; $TIME_DUMB + $TMP" | bc)
		# Heuristique RAND
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -rand $DIRECTORY/graph > $DIRECTORY/output.txt 2> /tmp/error
		if [ -s /tmp/error ]
		then
			echo "EXIT after error DUMB"
			exit 1
		fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND=$(echo "scale=3; $TIME_RAND + $TMP" | bc)
		# Heuristique MOMS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -moms $DIRECTORY/graph > $DIRECTORY/output.txt 2> /tmp/error
		if [ -s /tmp/error ]
		then
			echo "EXIT after error DUMB"
			exit 1
		fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS=$(echo "scale=3; $TIME_MOMS + $TMP" | bc)
		# Heuristique DLIS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -dlis $DIRECTORY/graph > $DIRECTORY/output.txt 2> /tmp/error
		if [ -s /tmp/error ]
		then
			echo "EXIT after error DUMB"
			exit 1
		fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS=$(echo "scale=3; $TIME_DLIS + $TMP" | bc)
		
	done
	TIME_DUMB=$(echo "scale=3; $TIME_DUMB / $NB_TEST" | bc)
	TIME_RAND=$(echo "scale=3; $TIME_RAND / $NB_TEST" | bc)	
	TIME_MOMS=$(echo "scale=3; $TIME_MOMS / $NB_TEST" | bc)	
	TIME_DLIS=$(echo "scale=3; $TIME_DLIS / $NB_TEST" | bc)	
	echo $nvar $TIME_DUMB $TIME_RAND $TIME_MOMS $TIME_DLIS >> $OUTPUT
# fin de la boucle
done

# gnuplot script-plot.p

# evince courbe1.pdf &

# Si une formule n'était pas satisfaite par l'affectation, on aura des "assert" dans le fichier
#grep "assert" $DIRECTORY/output.txt
