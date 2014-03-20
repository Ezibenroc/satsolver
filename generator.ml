open Printf

let nvar = ref 0 
let nclause = ref 0
let sclause = ref 0
let name_f = ref ""

;;
Random.self_init() ;
;;
exception Abort

(* On construit les clauses, ensemble de littéraux, à partir de tableaux *)
module Clauses =
struct
	type clause = { set : bool array ; mutable size : int }

	type literal = int

	(* Initialise une clause vide de taille n *)
	let init (n : int) : clause = {set = Array.make (2*n) false ; size = 0 }

	(* Test si la clause est vide *)
	let is_empty (c : clause) : bool = c.size = 0

	(* Test si l est présent dans la clause *)
	let mem (l : literal) (c : clause) : bool = if l = 0 then failwith "Null literal." else  c.set.(l+((Array.length c.set)/2-(if l > 0 then 1 else 0)))

	(* Ajoute l à la clause *)
	let add (l : literal) (c : clause) : unit = if l = 0 then failwith "Null literal." else let b = mem l c in c.set.(l+((Array.length c.set)/2-(if l > 0 then 1 else 0))) <- true ; c.size <- c.size + (if b then 0 else 1)

	(* Supprime l de la clause *)
	let remove (l : literal) (c : clause) : unit = if l = 0 then failwith "Null literal." else let b = mem l c in c.set.(l+((Array.length c.set)/2-(if l > 0 then 1 else 0))) <- false ; c.size <- c.size - (if b then 1 else 0)

	(* Union de clauses *)
	let union (c1 : clause) (c2 : clause) : clause =
		if Array.length c1.set <> Array.length c2.set then failwith "Different size."
		else (
		  let c = init ((Array.length c1.set)/2) in
		  for i = 0 to Array.length c1.set-1 do
		    c.set.(i) <- c1.set.(i) || c2.set.(i) ;
		    c.size <- c.size + (if c.set.(i) then 1 else 0)
		  done ;
		  c
		) 
	;;

	(* Représentation textuelle d'une clause *)
	let string_of_clause (c : clause) : string =
	  let s = ref "" in
		for i = 1 to (Array.length c.set)/2 do
		  if mem (-i) c then s := !s^(string_of_int (-i))^" " ;
		  if mem i c then s := !s^(string_of_int i)^" ";
		done ;
	  !s^"0"
		  ;;
	
	(* La clause est-elle une tautologie ? *)
	let is_tautology (c : clause) : bool =
	  let rec process (i : int) : bool =
	  	if i = 0 then false
	  	else if mem i c && mem (-i) c then raise Abort
	  	else process (i-1)
	  in try process !nvar with | Abort -> true | e -> raise e 
;;

	(* Retourne une copie "fraiche" de c *)
	let copy (c : clause) : clause = {set = Array.copy c.set ; size = c.size}
 
	(* Calcul la résolution de c1 et c2 par rapport à l *)
	(* Il y a un résolvant ssi l appartient à c1 et -l à c2, ou vice versa *)
	let resolution (c1 : clause) (c2 : clause) (l : literal) : clause option =
	  if (mem l c1 && mem (-l) c2) || (mem (-l) c1 && mem l c2) then 
	    let c = union c1 c2 in
	    remove l c ; remove (-l) c ;
	    if is_tautology c then None else Some c
	  else None
	  
	let clause_of_list (l : int list) : clause =
		let c = init !nvar in
		let rec process (l : int list) : unit =
			match l with
			| [] -> ()
			| h::t -> add h c ; process t
		in process l ;
		c
		
	(* Test l'égalité de clauses *)
	let equal (c1 : clause) (c2 : clause) : bool =
		if Array.length c1.set <> Array.length c2.set then failwith "Different array size"
		else if c1.size <> c2.size then false
		else
		let rec process (i : int) : bool =
			if i = Array.length c1.set then true
			else if c1.set.(i) <> c2.set.(i) then raise Abort else process (i+1)
		in try process 0 with | Abort -> false | e -> raise e
		
	(* Test si c1 est incluse dans c2 *)
	let included (c1 : clause) (c2 : clause) : bool =
		if Array.length c1.set <> Array.length c2.set then failwith "Different array size"
		else if c1.size > c2.size then false
		else
		let rec process (i : int) : bool =
			if i = Array.length c1.set then true
			else if not (c2.set.(i) || not c1.set.(i)) then raise Abort else process (i+1)
		in try process 0 with | Abort -> false | e -> raise e
		
	(* Test si un des littéraux de la liste est présent dans la clause *)
	let is_present (l : literal list) (c : clause) : bool =
		List.fold_left (fun acc x -> acc || mem x c) false l

	(* Génère un litéral aléatoirement *)
	let r_int (n : int) : int = (if Random.bool() then 1 else -1) * (Random.int(n) + 1) 

	(* Génère une clause de taille size, avec !nvar variables *)
	(* Pré-condition : size <= !nvar *)
	let gen_clause (size : int) : clause =
	  let c = init !nvar in
	  let rec process (s : int) : unit =
	    if s = 0 then ()
	    else ( 
	      let l = r_int !nvar in
		 if not (mem l c) && not (mem (-l) c) then (
		   add l c ; process (s-1)
		 )
		else process s 
	    )
	  in process size ; c
end


open Clauses

(* Supprime les clauses contenant une autre clause *)	
let rec clean_formula (f : clause list) : clause list =
	let rec process1 (c : Clauses.clause) (f : clause list) : clause list =
		match f with
		| [] -> [c]
		| h::t -> if Clauses.included c h then process1 c t else if Clauses.included h c then process1 h t else h::(process1 c t)
	in
	let rec process2 (f : clause list) (i : int) : clause list =
		if i = 0 then f
		else if f = [] then f
		else process2 (process1 (List.hd f) (List.tl f)) (i-1)
	in process2 f (List.length f)



(* Génère nb clauses de taille size *)
let rec gen_formula (nb : int) (size : int) : clause list =
  let rec process1 (n : int) : clause list =
    if n = 0 then []
    else (gen_clause size)::(process1 (n-1))
  in 
  let rec process2 (n : int) (l : clause list) : clause list =
    if n = 0 then l
    else let l2 = clean_formula((process1 n)@l) in
	 process2 (nb-(List.length l2)) (l2)
  in process2 nb []

(* Génère et affiche la formule au format DIMACS dans le fichier *)
let print_form (nb_clause : int) (size_clause : int) (name_file : string) : unit =
  let f = gen_formula nb_clause size_clause in
  let file = if name_file <> "" then open_out(name_file) else stdout in
  fprintf file "p cnf %d %d\n" !nvar (List.length f) ;
  List.iter (fun x -> fprintf file "%s\n" ((string_of_clause x))) f ;
  if(file <> stdout) then close_out(file)
  
(***********************)

let speclist = [
    ("-nvar", Arg.Int    (fun n -> nvar := n),  "the number of variables to use.");
    ("-nclause", Arg.Int    (fun n -> nclause := n),  "the number of clauses to use.");
    ("-sclause", Arg.Int    (fun n -> sclause := n),  "the size of the clauses to use.");
    ("-o", Arg.String (fun s -> name_f := s), ": output file (stdout while be used if not specified).")
]

(* Arrondi à n chiffres après la virgule (s représente un flottant) *)
let round (s : string) (n : int) : string =
	let x = String.index s '.' in
	try String.sub s 0 (x+n+1)
	with |_ -> s

let main() =
  Arg.parse speclist (fun x -> raise (Arg.Bad ("Bad argument : " ^ x))) "SYNTAXE :" ;
  if(!nvar <= 0) then failwith("Please provide the number of variables.") ;
  if(!nclause <= 0) then failwith("Please provide the number of clauses.") ;
  if(!sclause <= 0) then failwith("Please provide the size of clauses.") ;
  print_form !nclause !sclause !name_f ;
;;
main()
