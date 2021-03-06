open Printf

let nvar = ref 0 
let nclause = ref 0
let sclause = ref 0
let depth = ref 0
let depth_term = ref (-1)
let nedge = ref 0
let pathologic = ref false
let name_f = ref ""
let non_constant = ref false 

type mode_t = Clause_set | Formula | Graph | Unknown
type mode_formula_t = Difference | Congruence | Unknown_f
let mode = ref Unknown
let mode_formula = ref Unknown_f

let width_term = 5 
let int_max = 10

;;
Random.self_init() ;
;;
exception Abort


(** CLAUSES *********************************)

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

	(* Génère une clause de taille size, avec nb_var variables *)
	(* Pré-condition : size <= nb_var *)
	let gen_clause (nb_var : int) (size : int) : clause =
	  let c = init nb_var in
	  if !pathologic then add ((if Random.bool() then 1 else -1)*nb_var) c ;
	  let rec process (s : int) : unit =
	    if s = 0 then ()
	    else ( 
	      let l = r_int nb_var in
		 if not (mem l c) && not (mem (-l) c) then (
		   add l c ; process (s-1)
		 )
		else process s 
	    )
	  in process (size-(if !pathologic then 1 else 0)) ; c
end


open Clauses

(* Supprime les clauses contenant une autre clause *)	
let rec clean_clause_set (f : clause list) : clause list =
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
let gen_clause_set (nb_var : int) (nb : int) (size : int) : clause list =
  let rec process1 (n : int) : clause list =
    if n = 0 then []
    else (gen_clause nb_var (if !non_constant then ((Random.int size)+1) else size))::(process1 (n-1))
  in 
  let rec process2 (n : int) (l : clause list) : clause list =
    if n = 0 then l
    else let l2 = clean_clause_set((process1 n)@l) in
	 process2 (nb-(List.length l2)) (l2)
  in process2 nb []

exception Overflow

let among (k : int) (n : int) : int =
	let tab = Array.make_matrix (n+1) (n+1) 1 in
	for i=1 to n do
		for j = 1 to i-1 do
		  tab.(i).(j) <- tab.(i-1).(j-1) + tab.(i-1).(j) ;
		  if tab.(i).(j) <= 0 then raise Overflow ; 
		done ;
	done ;
	tab.(n).(k)

(* Vérifie qu'on peut générer un tel ensemble de clauses.
		Pour n variables, avec des clauses de taille k, on peut générer au plus (k parmi n)*2^k clauses *)
let check_generation_feasibility (nb_var : int) (nb_clause : int) (size_clause : int) : bool =
  try
    let a = among size_clause nb_var in
    let a = a*(int_of_float (2.**(float_of_int size_clause))) in
    a=0 || nb_clause <= a
  with | Overflow -> true | e -> raise e
	

(* Génère et affiche l'ensemble de clauses au format DIMACS dans le fichier (stdin si nom de fichier vide) *)
let print_clause_set (nb_var : int) (nb_clause : int) (size_clause : int) (name_file : string) : unit =
  if not(nb_var > 0 && nb_clause > 0 && size_clause > 0 && size_clause <= nb_var && check_generation_feasibility nb_var nb_clause size_clause) 
  then failwith "Incorrect number of variables and clauses, and size of clauses."
  else 
  let f = gen_clause_set nb_var nb_clause size_clause in
  let file = if name_file <> "" then open_out(name_file) else stdout in
  fprintf file "p cnf %d %d\n" nb_var (List.length f) ;
  List.iter (fun x -> fprintf file "%s\n" ((string_of_clause x))) f ;
  if(file <> stdout) then close_out(file)
  
  
(** FORMULAE ********************************)

type term =
    | Var of string
    | Fun of string * term list
    
type ineq =
    | Leq of string*string*int
    | Lt of string*string*int
    | Geq of string*string*int
    | Gt of string*string*int
    | Eq of string*string*int
    | Neq of string*string*int
    
type atom = Teq of term*term | Tneq of term*term | I of ineq | V of string

type formula = | Atom of atom
	       | And of formula * formula
	       | Or of formula * formula
	       | Imp of formula * formula
	       (*| Xor of formula * formula*)
	       | Not of formula

let rec string_of_term (t : term) : string =
    match t with
    | Var s -> s
    | Fun(s,l) -> s^"("^(List.fold_left (fun acc x -> acc^","^(string_of_term x)) (string_of_term (List.hd l)) (List.tl l))^")"

let string_of_ineq (i : ineq) : string =
  match i with
  | Leq(x1,x2,n) -> x1^"-"^x2^"<="^(string_of_int n)
  | Lt(x1,x2,n) -> x1^"-"^x2^"<"^(string_of_int n)
  | Geq(x1,x2,n) -> x1^"-"^x2^">="^(string_of_int n)
  | Gt(x1,x2,n) -> x1^"-"^x2^">"^(string_of_int n)
  | Eq(x1,x2,n) -> x1^"-"^x2^"="^(string_of_int n)
  | Neq(x1,x2,n) -> x1^"-"^x2^"!="^(string_of_int n)

let string_of_atom (a : atom) : string =
  match a with
  | Teq(t1,t2) -> (string_of_term t1)^"="^(string_of_term t2)
  | Tneq(t1,t2) -> (string_of_term t1)^"!="^(string_of_term t2)
  | I(i) -> string_of_ineq i
  | V(s) -> s


let rec string_of_formula (f : formula) : string =
  match f with
  | Atom(a) -> string_of_atom a
  | And(f1,f2) -> "("^(string_of_formula f1)^"/\\"^(string_of_formula f2)^")"
  | Or(f1,f2) -> "("^(string_of_formula f1)^"\\/"^(string_of_formula f2)^")"
  | Imp(f1,f2) -> "("^(string_of_formula f1)^"=>"^(string_of_formula f2)^")"
(*  | Xor(f1,f2) -> "("^(string_of_formula f1)^"+"^(string_of_formula f2)^")"*)
  | Not(f1) -> "~"^(string_of_formula f1)

(* nombre d'opérateurs différents *)
let nb_op = 4
let nb_diff = 6

(*
let rec var_of_int (n : int) : string =
  let s = String.make 1 (char_of_int ((int_of_char 'a') + (n mod 26))) in
  if n < 26 then s
  else (var_of_int (n/26))^s
*)

let var_of_int (n : int) : string = "x"^(string_of_int (n+1))
let fun_of_int (n : int) : string = "f"^(string_of_int (n+1))

(* Génère un atome, selon les options spécifiées *)
let rec gen_term(nvar : int) (depth : int) : term =
  if depth == 0 then Var(var_of_int (Random.int nvar))
  else let f = fun_of_int (Random.int nvar) in
       Fun(f,(gen_term_list nvar (depth-1) (Random.int width_term+1)))
and gen_term_list(nvar : int) (depth : int) (w : int) : term list =
  if w = 0 then []
  else (gen_term nvar depth)::(gen_term_list nvar depth (w-1))
  

let gen_diff (nvar : int) : ineq =
  let op = Random.int nb_diff and x1 = var_of_int (Random.int nvar) and x2 = var_of_int (Random.int nvar) and n = Random.int int_max in
  if op = 0 then Leq(x1,x2,n)
  else if op = 1 then Lt(x1,x2,n)
  else if op = 2 then Geq(x1,x2,n)
  else if op = 3 then Gt(x1,x2,n)
  else if op = 4 then Eq(x1,x2,n)
  else  Neq(x1,x2,n)

let gen_atom (nvar : int) (depth : int) : atom =
  match !mode_formula with
  | Unknown_f -> V(var_of_int(Random.int nvar))
  | Difference -> I(gen_diff nvar)
  | Congruence -> let op = Random.int 2 and t1 = gen_term nvar depth and t2 = gen_term nvar depth in
		  if op = 0 then Teq(t1,t2) else Tneq(t1,t2)

(* Génère une formule aléatoirement.
   nvar : nombre total de variables (pas forcément toutes utilisées)
   depth : profondeur de l'arbre représentant la formule *)
let rec gen_formula (nvar : int) (depth : int) (depth_term : int) : formula =
  if depth = 0 then Atom(gen_atom nvar depth_term)
  else let op = Random.int nb_op in
       let 	f1 = gen_formula nvar (depth-1) depth_term and
	   				f2 = gen_formula nvar (depth-1) depth_term in
       match op with
       | 0 -> And(f1,f2)
       | 1 -> Or(f1,f2)
       | 2 -> Imp(f1,f2)
 (*      | 3 -> Xor(f1,f2) *)
       | _ -> Not(f1)


       
(* Génère et affiche la formule dans le fichier (stdin si nom de fichier vide) *)
let print_formula (nb_var : int) (depth : int)(depth_term : int) (name_file : string) : unit =
  if not(nb_var > 0 && depth > 0) 
  then failwith "Incorrect depth and number of variables."
  else 
  let f = gen_formula nb_var depth depth_term in
  let file = if name_file <> "" then open_out(name_file) else stdout in
  fprintf file "%s\n" (string_of_formula f) ;
  if(file <> stdout) then close_out(file)


(** GRAPH ***********************************)

type pair = int * int

let eq_pair (p1 : pair) (p2 : pair) : bool =
  (fst p1 = fst p2 && snd p1 = snd p2) || (fst p1 = snd p2 && snd p1 = fst p2)

type graph = pair list 

(* Ajoute une arête au graphe (renvoie une exception si inccorect *)
let add_edge (p : pair) (g : graph) : graph =
  if fst p = snd p || List.exists (eq_pair p) g then raise Abort else p::g

(* Génère un graphe aléatoirement, de nvar noeuds et nedge arêtes. *)
let gen_graph (nvar : int) (nedge  : int) : graph =
  let rec process (nedge : int) (g : graph) : graph =
    if nedge = 0 then g
    else 
      try 
	process (nedge -1) (add_edge (((Random.int nvar)+1),((Random.int nvar)+1)) g)
      with | Abort -> process  nedge g |_ -> failwith ""
  in process nedge []

let string_of_pair (p : pair) : string = 
  "e "^(string_of_int (fst p))^" "^(string_of_int (snd p))

(* Génère et affiche le graphe dans le fichier (stdin si nom de fichier vide) *)
let print_graph (nb_var : int) (nb_edge : int) (name_file : string) : unit =
  if not(nb_var > 0 && nb_edge > 0 && (nb_var*(nb_var-1)/2 >= nb_edge)) 
  then failwith "Incorrect number of nodes and edges."
  else 
  let g = gen_graph nb_var nb_edge in
  let file = if name_file <> "" then open_out(name_file) else stdout in
  fprintf file "p edge %d %d\n" nb_var nb_edge ;
  List.iter (fun x -> fprintf file "%s\n" (string_of_pair x)) g ;
  if(file <> stdout) then close_out(file)

(** MAIN ************************************)

let speclist = [
    ("-nvar", Arg.Int    (fun n -> nvar := n),  "the number of variables to generate (clause set and formula generator).");
    ("-nclause", Arg.Int    (fun n -> if !mode <> Unknown && !mode <> Clause_set then failwith "Contradictory options." else (mode := Clause_set ; nclause := n)),  "the number of clauses to generate (clause set generator).");
    ("-sclause", Arg.Int    (fun n -> if !mode <> Unknown && !mode <> Clause_set then failwith "Contradictory options." else (mode := Clause_set ; sclause := n)),  "the size of the clauses to generate (clause set generator).");
    ("-depth", Arg.Int    (fun n -> if !mode <> Unknown && !mode <> Formula then failwith "Contradictory options." else (mode := Formula ; depth := n)),  "the depth of the formula to generate (formula generator).");
    ("-difference", Arg.Unit    (fun () -> if (!mode <> Unknown && !mode <> Formula) || (!mode_formula <> Unknown_f && !mode_formula <> Difference) then failwith "Contradictory options." else (mode := Formula ; mode_formula := Difference )),  "generates formula for difference theory.");
    ("-congruence", Arg.Int    (fun n -> if (!mode <> Unknown && !mode <> Formula) || (!mode_formula <> Unknown_f && !mode_formula <> Congruence) then failwith "Contradictory options." else (mode := Formula ; mode_formula := Congruence ; depth_term := n)),  "generates formula for congruence theory (terms of given size).");
    ("-nedge", Arg.Int    (fun n -> if !mode <> Unknown && !mode <> Graph then failwith "Contradictory options." else (mode := Graph ; nedge := n)),  "the number of edges to generate (graph generator).");
    ("-pathologic", Arg.Unit    (fun () -> pathologic := true),  "Generation of pathologic case for dumb heuristic.");
    ("-non_constant_sclause", Arg.Unit    (fun () -> if !mode <> Unknown && !mode <> Clause_set then failwith "Contradictory options." else (mode := Clause_set ; non_constant := true)),  "Generation of variable length clauses.");
    ("-o", Arg.String (fun s -> name_f := s), ": output file (stdout while be used if not specified).")
]

let main() =
  Arg.parse speclist (fun x -> raise (Arg.Bad ("Bad argument : " ^ x))) "SYNTAXE :" ;
  if !mode = Unknown then failwith "Please provide more options." ;
  if !nvar <= 0 then failwith "Please provide the number of variables." ;
  if !mode = Clause_set && !nclause <= 0 then failwith "Please provide the number of clauses." ;
  if !mode = Clause_set && !sclause <= 0 then failwith "Please provide the size of clauses." ;
  if !mode = Formula && !depth <= 0 then failwith "Please provide the depth of formula." ;
  if !mode = Graph && !nedge <= 0 then failwith "Please provide the number of edges of the graph." ;
  
  if !mode_formula = Congruence && !depth_term < 0 then failwith "Please provide the depth of the terms." ;



  if !mode = Clause_set then print_clause_set !nvar !nclause !sclause !name_f ;
  if !mode = Formula then print_formula !nvar !depth !depth_term !name_f ;
  if !mode = Graph then print_graph !nvar !nedge !name_f ;
;;
main()
