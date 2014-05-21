#include <sstream>
#include <cassert>

#include "structures/equality_atom.h"

#define EA theorysolver::EqualityAtom
#define SPEA std::shared_ptr<EA>

using namespace theorysolver;


bool EqualityAtom::is_atom_variable(std::string variable) {
    return variable.c_str()[0] == '#';
}
bool EqualityAtom::is_atom_literal(const std::map<int, std::string> &literal_to_name, unsigned int literal) {
    return EqualityAtom::is_atom_variable(literal_to_name.at(literal));
}

unsigned int EqualityAtom::atom_id_from_literal(const std::map<int, std::string> &variable_to_name, unsigned int literal) {
    unsigned int atom_id;
    atom_id = atoi(variable_to_name.at(literal).c_str()+1);
    return atom_id;
}

std::string EqualityAtom::variable_name_from_atom_id(unsigned long int atom_id) {
    return "#" + std::to_string(atom_id);
}
int EqualityAtom::literal_from_atom_id(const std::map<std::string, int> &name_to_variable, unsigned int atom_id) {
    return name_to_variable.at("#" + std::to_string(atom_id));
}

SPEA EqualityAtom::SPEA_from_literal(const EA_list &literal_to_EA, std::map<int, std::string> &variable_to_name, unsigned int literal) {
    unsigned int atom_id = EqualityAtom::atom_id_from_literal(variable_to_name, literal);
    assert(atom_id > 0);
    assert(atom_id <= literal_to_EA.size());
    return literal_to_EA[atom_id-1];
}

long unsigned int EqualityAtom::add_EA(EA_list &literal_to_EA, unsigned int left, Operator op, unsigned int right) {
    literal_to_EA.push_back(SPEA(new EA(left, op, right)));
    return literal_to_EA.size();
}
std::shared_ptr<EqualityAtom> EqualityAtom::SPEA_from_variable(const EA_list &literal_to_EA, std::string variable) {
    return literal_to_EA[atoi(variable.c_str()+1)-1];
}



EqualityAtom::EqualityAtom(unsigned int left, enum Operator op, unsigned int right) : left(left), right(right), op(op), canonical(NULL) {
}

std::string EqualityAtom::to_string() const {
    return "x" + std::to_string(this->left) + (this->op == EqualityAtom::EQUAL ? "=" : "!=") + "x" + std::to_string(this->right);
}
