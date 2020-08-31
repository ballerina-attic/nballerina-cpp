#include "BIR.h"

Param::Param() {}

Param::Param(Location pos, string name, int flags) {}

Param::~Param() {}

void Param::translate() {}

int Param::getParamFlag() {
  return flags;
}

string Param::getParamName()
{
  return name;
}

TypeDecl Param::gettypeDecl()
{
  return type;
}
