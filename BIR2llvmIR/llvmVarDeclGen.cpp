#include "BIR.h"

VarDecl::VarDecl (){}

VarDecl::VarDecl(string pname, string pmetaVarName) {
}

VarDecl::~VarDecl() {}

void VarDecl::translate() {}

int VarDecl::getinsOffset()
{
  return insOffset;
}

TypeDecl VarDecl::gettypeDecl() {
  return type;
}

string VarDecl::getvarName()
{
  return name;
}

bool VarDecl::getignoreVar() 
{
  return ignoreVariable;
}

VarKind VarDecl::getvarKind()
{
  return kind;
}
