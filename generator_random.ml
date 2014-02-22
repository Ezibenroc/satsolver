open Printf
open Sys
open Unix ;;

Random.self_init() ;;

let nvar = ref 0
let ntest = ref 0
let verbose = ref false

(* génère un litéral au hasard, où n est le nombre de variables *)
let r_int (n : int) : int = (if Random.bool() then 1 else -1) * (Random.int(n) + 1) 

let rec print(l : int list list) (f : out_channel) : unit = 
  List.iter (fun x -> List.iter (fun y -> fprintf f "%d " y) x ; fprintf f "0\n") l

(* Génère nb_clauses clauses, avec n variables *)
let rec compute (nb_clauses : int) (n : int) : int list list =
	if nb_clauses = 0 then []
	else
  	[r_int(n);r_int(n);r_int(n)]::(compute (nb_clauses -1) n)
  
  
;;

(* Ecrit une formule de n variables, générée aléatoirement, dans le fichier de nom s *)
let print_form(n : int)(s : string) : unit =
	let l = compute (n*4) n in
	let f = open_out(s) in
	fprintf f "p cnf %d %d\n" n (List.length l) ;
	print l f;
	close_out(f) ;;
	
let speclist = [
    ("-var", Arg.Int    (fun n -> nvar := n),  "the number of variables to use.");
    ("-test", Arg.Int    (fun n -> ntest := n),      ": the number of tests to run.");
    ("-v", Arg.Unit (fun ()-> verbose := true), ": verbosity.")
]

(* Arrondi à n chiffres après la virgule (s représente un flottant) *)
let round (s : string) (n : int) : string =
	let x = String.index s '.' in
	try String.sub s 0 (x+n+1)
	with |_ -> s

let main() =
  Arg.parse speclist (fun x -> raise (Arg.Bad ("Bad argument : " ^ x))) "SYNTAXE :" ;
  if(!nvar <= 0) then failwith("Please provide the number of variables.") ;
  if(!ntest <= 0) then failwith("Please provide the number of tests.");
  let x = gettimeofday() in
  for i = 1 to !ntest do
  	print_form !nvar "tmp" ;
  	if (command "./resol tmp > result") = 2 then failwith "Fatal error occured.";
  	if !verbose then print_endline("Test "^(string_of_int i)); 
  done ;
  print_endline("Average time : "^(round (string_of_float((gettimeofday()-.x)/.(float_of_int !ntest)))) 3) ;
;;
main()
