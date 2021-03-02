#include "BalOperand.h"
#include "BalType.h"
#include "BalVarDecl.h"

namespace nballerina {

Operand::Operand(VarDecl *vDecl) : varDecl(vDecl) {}

std::string Operand::getName() { return varDecl->getVarName(); }
int Operand::typeTag() { return varDecl->getTypeDecl()->getTypeTag(); }
VarDecl *Operand::getVarDecl() { return varDecl; }

} // namespace nballerina
