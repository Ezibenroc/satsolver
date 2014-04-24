#!/bin/bash

if [ $# != 1 ]
then
echo "Please provide the number of tests to perform."
exit 1
fi

NB_TEST=$1						    # nombre de tests pour chaque configuration
DIRECTORY=/tmp						# endroit de sauvegarde des fichiers temporaires
EXEC="./resol"						# executable
OUTPUT_WITHWL=clauses.nvar_3SAT.with_WL.dat
OUTPUT_WITHOUTWL=clauses.nvar_3SAT.without_WL.dat

rm -f $OUTPUT_WITHWL
rm -f $OUTPUT_WITHOUTWL
rm -f $DIRECTORY/output.txt
echo "Variable_number DUMB RAND MOMS DLIS DUMB_CL RAND_CL MOMS_CL DLIS_CL" >> $OUTPUT_WITHOUTWL
echo "Variable_number DUMB_WL RAND_WL MOMS_WL DLIS_WL DUMB_WL_CL RAND_WL_CL MOMS_WL_CL DLIS_WL_CL" >> $OUTPUT_WITHWL

for nb in `seq 10 250`; do
	TIME_DUMB=0
	TIME_RAND=0
	TIME_MOMS=0
	TIME_DLIS=0
	
	TIME_DUMB_WL=0
	TIME_RAND_WL=0
	TIME_MOMS_WL=0
	TIME_DLIS_WL=0
	
	TIME_DUMB_CL=0
	TIME_RAND_CL=0
	TIME_MOMS_CL=0
	TIME_DLIS_CL=0
	
	TIME_DUMB_WL_CL=0
	TIME_RAND_WL_CL=0
	TIME_MOMS_WL_CL=0
	TIME_DLIS_WL_CL=0

  echo "Computing test for " $nb " variables."

	# On fait plusieurs tests par taille
	for test in `seq 1 $NB_TEST` ; do
		echo -e "\t\tTest $test"
		# Génération de la formule dans le fichier $DIRECTORY/formula.cnf
		A=$(echo "scale=0; $nb * 4.26 / 1" | bc) # on divise par 1 afin que bc fasse l'arrondi
		./generator -nvar $nb -nclause $A -sclause 3 -o $DIRECTORY/formula.cnf 

        minisat $DIRECTORY/formula.cnf > /dev/null
        MINISAT_RESULT=$?   # 10 si satisfiable, 20 si insatisfiable, autre chose si erreur de format


		# Résolution de la formule (en vérifiant la correction de la solution)
	
		# Heuristique DUMB
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DUMB."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB=$(echo "scale=3; $TIME_DUMB + $TMP" | bc)
		
		# Heuristique RAND
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -rand  $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error RAND."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND=$(echo "scale=3; $TIME_RAND + $TMP" | bc)
		
		# Heuristique MOMS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -moms  $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error MOMS."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS=$(echo "scale=3; $TIME_MOMS + $TMP" | bc)
		
		# Heuristique DLIS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -dlis  $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DLIS."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS=$(echo "scale=3; $TIME_DLIS + $TMP" | bc)
		
		# La même chose, avec les watched literals
		
		# Heuristique DUMB_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DUMB_WL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB_WL=$(echo "scale=3; $TIME_DUMB_WL + $TMP" | bc)
		
		# Heuristique RAND_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -rand $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error RAND_WL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND_WL=$(echo "scale=3; $TIME_RAND_WL + $TMP" | bc)
		
		# Heuristique MOMS_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -moms $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error MOMS_WL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS_WL=$(echo "scale=3; $TIME_MOMS_WL + $TMP" | bc)
		
		# Heuristique DLIS_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -dlis $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DLIS_WL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS_WL=$(echo "scale=3; $TIME_DLIS_WL + $TMP" | bc)
		
		
		
		
		#############################
		#                           #
		# IDEM AVEC CLAUSE LEARNING #
		#                           #
		#############################
		
		# Heuristique DUMB_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DUMB_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB_CL=$(echo "scale=3; $TIME_DUMB_CL + $TMP" | bc)
		
		# Heuristique RAND_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -rand -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error RAND_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND_CL=$(echo "scale=3; $TIME_RAND_CL + $TMP" | bc)
		
		# Heuristique MOMS_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -moms -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error MOMS_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS_CL=$(echo "scale=3; $TIME_MOMS_CL + $TMP" | bc)
		
		# Heuristique DLIS_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -dlis -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DLIS_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS_CL=$(echo "scale=3; $TIME_DLIS_CL + $TMP" | bc)
		
		# La même chose, avec les watched literals
		
		# Heuristique DUMB_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DUMB_WL_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB_WL_CL=$(echo "scale=3; $TIME_DUMB_WL_CL + $TMP" | bc)
		
		# Heuristique RAND_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -rand -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error RAND_WL_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND_WL_CL=$(echo "scale=3; $TIME_RAND_WL_CL + $TMP" | bc)
		
		# Heuristique MOMS_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -moms -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error MOMS_WL_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS_WL_CL=$(echo "scale=3; $TIME_MOMS_WL_CL + $TMP" | bc)
		
		# Heuristique DLIS_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -dlis -CL $DIRECTORY/formula.cnf | ./check_result.py $DIRECTORY/formula.cnf > /dev/null
		RESOL_RESULT=$?     # 10 si satisfiable et assignation correcte, 20 si insatisfiable (code erreur de check_result), 1 si assignation incorrecte
            if [ $MINISAT_RESULT -ne $RESOL_RESULT ] # résultats différents
            then
	            echo "Fatal error DLIS_WL_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS_WL_CL=$(echo "scale=3; $TIME_DLIS_WL_CL + $TMP" | bc)		
		
		
		
		
		
		
	done
	TIME_DUMB=$(echo "scale=3; $TIME_DUMB / $NB_TEST" | bc)
	TIME_RAND=$(echo "scale=3; $TIME_RAND / $NB_TEST" | bc)	
	TIME_MOMS=$(echo "scale=3; $TIME_MOMS / $NB_TEST" | bc)	
	TIME_DLIS=$(echo "scale=3; $TIME_DLIS / $NB_TEST" | bc)	
	
	TIME_DUMB_WL=$(echo "scale=3; $TIME_DUMB_WL / $NB_TEST" | bc)
	TIME_RAND_WL=$(echo "scale=3; $TIME_RAND_WL / $NB_TEST" | bc)	
	TIME_MOMS_WL=$(echo "scale=3; $TIME_MOMS_WL / $NB_TEST" | bc)	
	TIME_DLIS_WL=$(echo "scale=3; $TIME_DLIS_WL / $NB_TEST" | bc)	
	
	TIME_DUMB_CL=$(echo "scale=3; $TIME_DUMB_CL / $NB_TEST" | bc)
	TIME_RAND_CL=$(echo "scale=3; $TIME_RAND_CL / $NB_TEST" | bc)	
	TIME_MOMS_CL=$(echo "scale=3; $TIME_MOMS_CL / $NB_TEST" | bc)	
	TIME_DLIS_CL=$(echo "scale=3; $TIME_DLIS_CL / $NB_TEST" | bc)	
	
	TIME_DUMB_WL_CL=$(echo "scale=3; $TIME_DUMB_WL_CL / $NB_TEST" | bc)
	TIME_RAND_WL_CL=$(echo "scale=3; $TIME_RAND_WL_CL / $NB_TEST" | bc)	
	TIME_MOMS_WL_CL=$(echo "scale=3; $TIME_MOMS_WL_CL / $NB_TEST" | bc)	
	TIME_DLIS_WL_CL=$(echo "scale=3; $TIME_DLIS_WL_CL / $NB_TEST" | bc)		
	echo $nb $TIME_DUMB $TIME_RAND $TIME_MOMS $TIME_DLIS $TIME_DUMB_CL $TIME_RAND_CL $TIME_MOMS_CL $TIME_DLIS_CL >> $OUTPUT_WITHOUTWL
	echo $nb $TIME_DUMB_WL $TIME_RAND_WL $TIME_MOMS_WL $TIME_DLIS_WL $TIME_DUMB_WL_CL $TIME_RAND_WL_CL $TIME_MOMS_WL_CL $TIME_DLIS_WL_CL >> $OUTPUT_WITHWL
# fin de la boucle
done

# gnuplot script-plot.p

# evince courbe1.pdf &

# Si une formule n'était pas satisfaite par l'affectation, on aura des "assert" dans le fichier
#grep "assert" $DIRECTORY/output.txt
