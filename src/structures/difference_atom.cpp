#include <cassert>
#include "structures/difference_atom.h"

using namespace theorysolver;

#define DA theorysolver::DifferenceAtom
#define SPDA std::shared_ptr<DA>

bool DifferenceAtom::is_atom_variable(std::string variable) {
    return variable.c_str()[0] == '#';
}
bool DifferenceAtom::is_atom_literal(const std::map<int, std::string> &literal_to_name, unsigned int literal) {
    return DifferenceAtom::is_atom_variable(literal_to_name.at(literal));
}

unsigned int DifferenceAtom::atom_id_from_literal(const std::map<int, std::string> &variable_to_name, unsigned int literal) {
    unsigned int atom_id;
    atom_id = atoi(variable_to_name.at(literal).c_str()+1);
    return atom_id;
}

std::string DifferenceAtom::variable_name_from_atom_id(unsigned long int atom_id) {
    return "#" + std::to_string(atom_id);
}
int DifferenceAtom::literal_from_atom_id(const std::map<std::string, int> &name_to_variable, unsigned int atom_id) {
    return name_to_variable.at("#" + std::to_string(atom_id));
}

SPDA DifferenceAtom::SPDA_from_literal(const DA_list &literal_to_DA, std::map<int, std::string> &variable_to_name, unsigned int literal) {
    unsigned int atom_id = DifferenceAtom::atom_id_from_literal(variable_to_name, literal);
    assert(atom_id > 0);
    assert(atom_id <= literal_to_DA.size());
    return literal_to_DA[atom_id-1];
}

long unsigned int DifferenceAtom::add_DA(DA_list &literal_to_DA, unsigned int i, unsigned int j, DifferenceAtom::Operator op, int n) {
    literal_to_DA.push_back(SPDA(new DA(i, j, op, n)));
    return literal_to_DA.size();
}
std::shared_ptr<DifferenceAtom> DifferenceAtom::SPDA_from_variable(const DA_list &literal_to_DA, std::string variable) {
    return literal_to_DA[atoi(variable.c_str()+1)-1];
}

DifferenceAtom::DifferenceAtom(unsigned int i, enum Operator op, int n) : i(i), j(0), op(op), n(n), canonical(NULL) {
}
DifferenceAtom::DifferenceAtom(unsigned int i, unsigned int j, enum Operator op, int n) : i(i), j(j), op(op), n(n), canonical(NULL) {
}

std::string DifferenceAtom::to_string() const {
    std::string s("x" + std::to_string(this->i)), op;
    if (this->j)
        s += " - x" + std::to_string(this->j);
    switch (this->op) {
        case LOWER: op = "<"; break;
        case GREATER: op = ">"; break;
        case LEQ: op = "<="; break;
        case GEQ: op = ">="; break;
        case EQUAL: op = "="; break;
        case UNEQUAL: op = "!="; break;
        default: assert(false);
    }
    s += " " + op + " " + std::to_string(n);
    return s;
}
