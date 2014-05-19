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

set title "Génération de formules (logique de différence, 5 variables), avec WL"
set xlabel "Profondeur"
plot for [IDX=2:9] "diff_logic.depth.with_WL_5var.dat" using 1:IDX title columnheader(IDX)

set title "Génération de formules (logique de différence, 5 variables), sans WL"
set xlabel "Profondeur"
plot for [IDX=2:9] "diff_logic.depth.without_WL_5var.dat" using 1:IDX title columnheader(IDX)

set title "Génération de formules (logique de différence, 50 variables), avec WL"
set xlabel "Profondeur"
plot for [IDX=2:9] "diff_logic.depth.with_WL_50var.dat" using 1:IDX title columnheader(IDX)

set title "Génération de formules (logique de différence, 50 variables), sans WL"
set xlabel "Profondeur"
plot for [IDX=2:9] "diff_logic.depth.without_WL_50var.dat" using 1:IDX title columnheader(IDX)



#set title "Génération de clauses (3SAT), avec WL"
#set xlabel "Nombre de variables"
#plot for [IDX=2:9] "clauses.nvar_3SAT.with_WL.dat" using 1:IDX title columnheader(IDX)

#set title "Génération de clauses (3SAT), sans WL"
#set xlabel "Nombre de variables"
#plot for [IDX=2:9] "clauses.nvar_3SAT.without_WL.dat" using 1:IDX title columnheader(IDX)

#set title "Génération de clauses (5SAT), avec WL"
#set xlabel "Nombre de variables"
#plot for [IDX=2:9] "clauses.nvar_5SAT.with_WL.dat" using 1:IDX title columnheader(IDX)

#set title "Génération de clauses (5SAT), sans WL"
#set xlabel "Nombre de variables"
#plot for [IDX=2:9] "clauses.nvar_5SAT.without_WL.dat" using 1:IDX title columnheader(IDX)



#set title "Génération de formules"
#set xlabel "Profondeur"
#plot for [IDX=2:9] "formulae.depth.dat" using 1:IDX title columnheader(IDX)

#set title "Génération de graphe"
#set xlabel "Nombre de nœuds"
#plot for [IDX=2:9] "graph.nvar.dat" using 1:IDX title columnheader(IDX)
