# pour visualiser le dessin trace par ce script gnuplot, taper
# gnuplot -persist script-plot.p
#  (en s`assurant que le fichier comparaison.dat est present dans le repertoire)

reset

### decommenter les 2 lignes ci-dessous pour engendrer un fichier pdf
### plutot qu`un dessin a l`ecran
 set term pdfcairo
 set output "courbes.pdf" # le nom du fichier qui est engendre

set ylabel "Temps d'execution"


# Dessin en joignant des points
set style data linespoints

set pointsize 0.5   # la taille des points


# on trace deux courbes: avec les colonnes 1 et 2, avec les colonnes 1 et 3
# a chaque fois, le nom de la courbe est lu en tete de colonne
set title "Génération de clauses"
#set xlabel "Nombre de clauses"
#plot for [IDX=2:5] "clauses.nclause.dat" using 1:IDX title columnheader(IDX)
set xlabel "Nombre de variables"
plot for [IDX=2:9] "clauses.nvar.dat" using 1:IDX title columnheader(IDX)

#set title "Génération de formules"
#set xlabel "Profondeur"
#plot for [IDX=2:5] "formulae.depth.dat" using 1:IDX title columnheader(IDX)

#set title "Génération de graphe"
#set xlabel "Nombre de nœuds"
#plot for [IDX=2:5] "graph.nvar.dat" using 1:IDX title columnheader(IDX)
