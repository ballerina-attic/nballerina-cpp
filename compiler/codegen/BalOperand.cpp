#include "BalOperand.h"

namespace nballerina {
Operand::Operand(VarDecl *vDecl) : varDecl(vDecl) {}

std::string Operand::name() { return varDecl->getVarName(); }
int Operand::typeTag() { return varDecl->getTypeDecl()->getTypeTag(); }
VarDecl *Operand::getVarDecl() { return varDecl; }
void Operand::setVarDecl(VarDecl *newDecl) { varDecl = newDecl; }
} // namespace nballerina
