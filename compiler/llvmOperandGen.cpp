#include "BIR.h"

Operand::Operand(VarDecl *vDecl) : varDecl(vDecl) {}

string Operand::name() { return varDecl->getVarName(); }

int Operand::typeTag() { return varDecl->getTypeDecl()->getTypeTag(); }
