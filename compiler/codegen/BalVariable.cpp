#include "BalVariable.h"

namespace nballerina {

Variable::Variable(Type *_type, std::string _name, VarKind _kind)
    : VariableKind(_name, _kind), type(_type) {}

Type *Variable::getTypeDecl() { return type; }

} // namespace nballerina
