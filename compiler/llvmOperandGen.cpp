#include "BIR.h"

Operand::Operand() {}

Operand::Operand(VarDecl *vDecl) : varDecl(vDecl) {}

Operand::~Operand() {}

string Operand::name() { return varDecl->getVarName(); }

int Operand::typeTag() { return varDecl->getTypeDecl()->getTypeTag(); }

void Operand::translate(LLVMModuleRef &modRef) {}
