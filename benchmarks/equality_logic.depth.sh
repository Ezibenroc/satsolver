#!/bin/bash

if [ $# != 1 ]
then
echo "Please provide the number of tests to perform."
exit 1
fi

NB_VAR=5
NB_TEST=$1						    # nombre de tests pour chaque configuration
DIRECTORY=/tmp						# endroit de sauvegarde des fichiers temporaires
EXEC="./resol_equal"				# executable
OUTPUT_WITHWL="eq_logic.depth.with_WL_"$NB_VAR"var.dat"
OUTPUT_WITHOUTWL="eq_logic.depth.without_WL_"$NB_VAR"var.dat"


rm -f $OUTPUT_WITHWL
rm -f $OUTPUT_WITHOUTWL
rm -f $DIRECTORY/output.txt
echo "Depth DUMB RAND MOMS DLIS DUMB_CL RAND_CL MOMS_CL DLIS_CL" >> $OUTPUT_WITHOUTWL
echo "Depth DUMB_WL RAND_WL MOMS_WL DLIS_WL DUMB_WL_CL RAND_WL_CL MOMS_WL_CL DLIS_WL_CL" >> $OUTPUT_WITHWL

for depth in `seq 1 20`; do
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

  echo "Computing test for depth " $depth"."

	# On fait plusieurs tests par taille
	for test in `seq 1 $NB_TEST` ; do
		echo -e "\t\tTest $test"
		# Génération de la formule dans le fichier $DIRECTORY/formula
		./generator -nvar $NB_VAR -depth $depth -congruence 0 -o $DIRECTORY/formula 


		# Résolution de la formule (en vérifiant la correction de la solution)
	
		# Heuristique DUMB
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error DUMB."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB=$(echo "scale=3; $TIME_DUMB + $TMP" | bc)
		
		# Heuristique RAND
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -rand  $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error RAND."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND=$(echo "scale=3; $TIME_RAND + $TMP" | bc)
		
		# Heuristique MOMS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -moms  $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error MOMS."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS=$(echo "scale=3; $TIME_MOMS + $TMP" | bc)
		
		# Heuristique DLIS
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -dlis  $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error DLIS."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS=$(echo "scale=3; $TIME_DLIS + $TMP" | bc)
		
		# La même chose, avec les watched literals
		
		# Heuristique DUMB_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error DUMB_WL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB_WL=$(echo "scale=3; $TIME_DUMB_WL + $TMP" | bc)
		
		# Heuristique RAND_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -rand $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error RAND_WL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND_WL=$(echo "scale=3; $TIME_RAND_WL + $TMP" | bc)
		
		# Heuristique MOMS_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -moms $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error MOMS_WL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS_WL=$(echo "scale=3; $TIME_MOMS_WL + $TMP" | bc)
		
		# Heuristique DLIS_WL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -dlis $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
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
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error DUMB_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB_CL=$(echo "scale=3; $TIME_DUMB_CL + $TMP" | bc)
		
		# Heuristique RAND_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -rand -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error RAND_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND_CL=$(echo "scale=3; $TIME_RAND_CL + $TMP" | bc)
		
		# Heuristique MOMS_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -moms -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error MOMS_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS_CL=$(echo "scale=3; $TIME_MOMS_CL + $TMP" | bc)
		
		# Heuristique DLIS_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -dlis -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error DLIS_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DLIS_CL=$(echo "scale=3; $TIME_DLIS_CL + $TMP" | bc)
		
		# La même chose, avec les watched literals
		
		# Heuristique DUMB_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error DUMB_WL_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_DUMB_WL_CL=$(echo "scale=3; $TIME_DUMB_WL_CL + $TMP" | bc)
		
		# Heuristique RAND_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -rand -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error RAND_WL_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_RAND_WL_CL=$(echo "scale=3; $TIME_RAND_WL_CL + $TMP" | bc)
		
		# Heuristique MOMS_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -moms -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
            then
	            echo "Fatal error MOMS_WL_CL."
	            exit 1
            fi
		TMP=`cat $DIRECTORY/result.txt`
		TIME_MOMS_WL_CL=$(echo "scale=3; $TIME_MOMS_WL_CL + $TMP" | bc)
		
		# Heuristique DLIS_WL_CL
		/usr/bin/time --quiet -f'%U' -o $DIRECTORY/result.txt $EXEC -WL -dlis -CL $DIRECTORY/formula  > /dev/null
		RESOL_RESULT=$?     
            if [ 0 -ne $RESOL_RESULT ] # résultats incorrects
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
	echo $depth $TIME_DUMB $TIME_RAND $TIME_MOMS $TIME_DLIS $TIME_DUMB_CL $TIME_RAND_CL $TIME_MOMS_CL $TIME_DLIS_CL >> $OUTPUT_WITHOUTWL
	echo $depth $TIME_DUMB_WL $TIME_RAND_WL $TIME_MOMS_WL $TIME_DLIS_WL $TIME_DUMB_WL_CL $TIME_RAND_WL_CL $TIME_MOMS_WL_CL $TIME_DLIS_WL_CL >> $OUTPUT_WITHWL
# fin de la boucle
done

# gnuplot script-plot.p

# evince courbe1.pdf &

# Si une formule n'était pas satisfaite par l'affectation, on aura des "assert" dans le fichier
#grep "assert" $DIRECTORY/output.txt
