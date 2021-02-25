#include "BalVarDecl.h"

namespace nballerina {

VarDecl::VarDecl(std::string name) : varName(name) {}

VarDecl::VarDecl(TypeDecl *ty, std::string name, VarKind k)
    : type(ty), varName(name), kind(k) {}

TypeDecl *VarDecl::getTypeDecl() { return type; }
VarKind VarDecl::getVarKind() { return kind; }
std::string VarDecl::getVarName() { return varName; }

void VarDecl::setTypeDecl(TypeDecl *newType) { type = newType; }
void VarDecl::setVarKind(VarKind newKind) { kind = newKind; }

} // namespace nballerina
