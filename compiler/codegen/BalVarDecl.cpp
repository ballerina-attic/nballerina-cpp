#include "BalVarDecl.h"

namespace nballerina {

VarDecl::VarDecl(Type *_type, std::string _name, VarKind _kind)
    : type(_type), name(_name), kind(_kind) {}

Type *VarDecl::getTypeDecl() { return type; }
VarKind VarDecl::getVarKind() { return kind; }
std::string VarDecl::getVarName() { return name; }

} // namespace nballerina
