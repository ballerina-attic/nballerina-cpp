#include "BalOperand.h"

namespace nballerina {

Operand::Operand(std::string _name, VarKind _kind) : name(_name), kind(_kind) {}

std::string Operand::getName() { return name; }
VarKind Operand::getKind() { return kind; }

FunctionParam::FunctionParam(std::string _name)
    : Operand(_name, ARG_VAR_KIND), type(nullptr) {}

Type *FunctionParam::getType() { return type; }
void FunctionParam::setType(Type *_type) { type = _type; }

} // namespace nballerina
