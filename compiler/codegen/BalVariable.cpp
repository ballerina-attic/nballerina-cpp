#include "BalVariable.h"

namespace nballerina {

Variable::Variable(Type *_type, std::string _name, VarKind _kind)
    : type(_type), name(_name), kind(_kind) {}

Type *Variable::getTypeDecl() { return type; }
VarKind Variable::getVarKind() { return kind; }
std::string Variable::getVarName() { return name; }

} // namespace nballerina
