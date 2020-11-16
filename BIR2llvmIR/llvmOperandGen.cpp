#include "BIR.h"

Operand::Operand() {}

Operand::Operand(VarDecl *vDecl):varDecl(vDecl) {}

Operand::~Operand() {}

void Operand::translate(LLVMModuleRef &modRef) {}
