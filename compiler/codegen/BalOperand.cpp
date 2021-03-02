#include "BalOperand.h"

namespace nballerina {

Operand::Operand(std::string _name, VarKind _kind) : name(_name), kind(_kind) {}

std::string Operand::getName() { return name; }
VarKind Operand::getKind() { return kind; }

Type *Operand::getType() { return type; }
void Operand::setType(Type *_type) { type = _type; }

} // namespace nballerina
