#include <sstream>
#include <cassert>

#include "structures/congruence_atom.h"

#define CA theorysolver::CongruenceAtom
#define SPCA std::shared_ptr<CA>

using namespace theorysolver;

Term::Term(unsigned int symbol_id) : type(Term::SYMBOL), symbol_id(symbol_id), function_name(), arguments() {
}
Term::Term(std::string function_name, std::vector<std::shared_ptr<Term>> arguments) : type(Term::FUNCTION), symbol_id(0), function_name(function_name), arguments(arguments) {
}
Term::Term(const Term &t) : type(t.type), symbol_id(t.symbol_id), function_name(t.function_name), arguments(t.arguments) {
}

std::string Term::to_string() const {
    if (this->type == Term::SYMBOL)
        return "x" + std::to_string(this->symbol_id);
    else {
        std::ostringstream oss;
        oss << this->function_name + "(";
        for (unsigned i=0; i<this->arguments.size(); i++) {
            if (i)
                oss << ", ";
            oss << "x" + this->arguments[i]->to_string();
        }
        oss << ")";
        return oss.str();
    }
}





bool CongruenceAtom::is_atom_variable(std::string variable) {
    return variable.c_str()[0] == '#';
}
bool CongruenceAtom::is_atom_literal(const std::map<int, std::string> &literal_to_name, unsigned int literal) {
    return CongruenceAtom::is_atom_variable(literal_to_name.at(literal));
}

unsigned int CongruenceAtom::atom_id_from_literal(const std::map<int, std::string> &variable_to_name, unsigned int literal) {
    unsigned int atom_id;
    atom_id = atoi(variable_to_name.at(literal).c_str()+1);
    return atom_id;
}

std::string CongruenceAtom::variable_name_from_atom_id(unsigned long int atom_id) {
    return "#" + std::to_string(atom_id);
}
int CongruenceAtom::literal_from_atom_id(const std::map<std::string, int> &name_to_variable, unsigned int atom_id) {
    return name_to_variable.at("#" + std::to_string(atom_id));
}

SPCA CongruenceAtom::SPCA_from_literal(const CA_list &literal_to_CA, std::map<int, std::string> &variable_to_name, unsigned int literal) {
    unsigned int atom_id = CongruenceAtom::atom_id_from_literal(variable_to_name, literal);
    assert(atom_id > 0);
    assert(atom_id <= literal_to_CA.size());
    return literal_to_CA[atom_id-1];
}

long unsigned int CongruenceAtom::add_CA(CA_list &literal_to_CA, Term &left, Operator op, Term &right) {
    literal_to_CA.push_back(SPCA(new CA(std::shared_ptr<Term>(new Term(left)), op, std::shared_ptr<Term>(new Term(right)))));
    return literal_to_CA.size();
}
std::shared_ptr<CongruenceAtom> CongruenceAtom::SPCA_from_variable(const CA_list &literal_to_CA, std::string variable) {
    return literal_to_CA[atoi(variable.c_str()+1)-1];
}

bool Term::operator==(const Term &that) const {
    if (this->type != that.type)
        return false;
    else if (this->type == Term::SYMBOL)
        return this->symbol_id == that.symbol_id;
    else if (this->type == Term::FUNCTION) {
        if (this->arguments.size() != that.arguments.size())
            return false;
        if (this->function_name != that.function_name)
            return false;
        for (unsigned i=0; i<this->arguments.size(); i++)
            if (!(this->arguments[i] == that.arguments[i]))
                return false;
        return true;
    }
    else
        assert(false);
}



CongruenceAtom::CongruenceAtom(std::shared_ptr<Term> left, enum Operator op, std::shared_ptr<Term> right) : left(left), right(right), op(op) {
}

std::string CongruenceAtom::to_string() const {
    return this->left->to_string() + (this->op == CongruenceAtom::EQUAL ? "=" : "!=") + this->right->to_string();
}
