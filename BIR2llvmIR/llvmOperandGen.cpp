#include "BIR.h"

Operand::Operand()
{
}

Operand::Operand(VarDecl *vDecl):varDecl(vDecl)
{
}
Operand::~Operand()
{
}

void Operand::translate()
{
}

VarDecl* Operand::getvarDecl() {
  return varDecl;
}
