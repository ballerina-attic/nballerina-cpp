#include "BalOperand.h"

namespace nballerina {

Operand::Operand(std::string _name, VarKind _kind) : VariableKind(_name, _kind) {}

FunctionParam::FunctionParam(std::string _name) : Operand(_name, ARG_VAR_KIND), type(nullptr) {}

Type *FunctionParam::getType() { return type; }
void FunctionParam::setType(Type *_type) { type = _type; }

} // namespace nballerina
