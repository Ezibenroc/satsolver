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

set pointsize 0.1   # la taille des points

set logscale y

set title "Génération de clauses (3SAT)"
set xlabel "Nombre de variables"
plot for [IDX=2:9] "clauses.nvar_3SAT.dat" using 1:IDX title columnheader(IDX)

set title "Génération de clauses (5SAT)"
set xlabel "Nombre de variables"
plot for [IDX=2:9] "clauses.nvar_5SAT.dat" using 1:IDX title columnheader(IDX)

set title "Génération de formules"
set xlabel "Profondeur"
plot for [IDX=2:9] "formulae.depth.dat" using 1:IDX title columnheader(IDX)

set title "Génération de graphe"
set xlabel "Nombre de nœuds"
plot for [IDX=2:9] "graph.nvar.dat" using 1:IDX title columnheader(IDX)
