#include "BalVarDecl.h"

namespace nballerina {

VarDecl::VarDecl(std::string name) : varName(name) {}

VarDecl::VarDecl(Type *ty, std::string name, VarKind k)
    : type(ty), varName(name), kind(k) {}

Type *VarDecl::getTypeDecl() { return type; }
VarKind VarDecl::getVarKind() { return kind; }
std::string VarDecl::getVarName() { return varName; }

void VarDecl::setTypeDecl(Type *newType) { type = newType; }
void VarDecl::setVarKind(VarKind newKind) { kind = newKind; }

} // namespace nballerina
