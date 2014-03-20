#!/bin/bash

if [ $# != 1 ]
then
echo "Please provide the number of tests to perform."
exit 1
fi

NB_TEST=$1

# preparation du fichier comparaison.dat: on l'enleve s'il existe, 
rm -f comparaison.dat
echo "Clause_number DUMB RAND MOMS DLIS MINISAT" >> comparaison.dat

for nb in `seq 40 50`; do
	TIME_DUMB=0
	TIME_RAND=0
	TIME_MOMS=0
	TIME_DLIS=0

  echo "Computing test for " $nb " clauses."

	# On fait plusieurs tests par taille
	for test in `seq 1 $NB_TEST` ; do
		# Génération de la formule dans le fichier /tmp/formula.cnf
		A=`expr $nb \* 4`
		./generator -nvar $nb -nclause $A -sclause 3 -o /tmp/formula.cnf

		# Résolution de la formule (en vérifiant la correction de la solution)
	
		# Heuristique DUMB
		/usr/bin/time --quiet -f'%U' -o /tmp/result.txt ./resol /tmp/formula.cnf | ./check_result.py /tmp/formula.cnf >> /tmp/output.txt
		TMP=`cat /tmp/result.txt`
		TIME_DUMB=$(echo "scale=3; $TIME_DUMB + $TMP" | bc)
		# Heuristique RAND
		/usr/bin/time --quiet -f'%U' -o /tmp/result.txt ./resol -rand /tmp/formula.cnf | ./check_result.py /tmp/formula.cnf >> /tmp/output.txt
		TMP=`cat /tmp/result.txt`
		TIME_RAND=$(echo "scale=3; $TIME_RAND + $TMP" | bc)
		# Heuristique MOMS
		/usr/bin/time --quiet -f'%U' -o /tmp/result.txt ./resol -moms /tmp/formula.cnf | ./check_result.py /tmp/formula.cnf >> /tmp/output.txt
		TMP=`cat /tmp/result.txt`
		TIME_MOMS=$(echo "scale=3; $TIME_MOMS + $TMP" | bc)
		# Heuristique DLIS
		/usr/bin/time --quiet -f'%U' -o /tmp/result.txt ./resol -dlis /tmp/formula.cnf | ./check_result.py /tmp/formula.cnf >> /tmp/output.txt
		TMP=`cat /tmp/result.txt`
		TIME_DLIS=$(echo "scale=3; $TIME_DLIS + $TMP" | bc)
		
	done
	TIME_DUMB=$(echo "scale=3; $TIME_DUMB / $NB_TEST" | bc)
	TIME_RAND=$(echo "scale=3; $TIME_RAND / $NB_TEST" | bc)	
	TIME_MOMS=$(echo "scale=3; $TIME_MOMS / $NB_TEST" | bc)	
	TIME_DLIS=$(echo "scale=3; $TIME_DLIS / $NB_TEST" | bc)	
	echo $nb $TIME_DUMB $TIME_RAND $TIME_MOMS $TIME_DLIS >> ./comparaison.dat 
# fin de la boucle
done

gnuplot script-plot.p

evince courbe1.pdf &

# Si une formule n'était pas satisfaite par l'affectation, on aura des "assert" dans le fichier
#grep "assert" /tmp/output.txt
