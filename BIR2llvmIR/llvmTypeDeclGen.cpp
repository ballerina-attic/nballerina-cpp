#include "BIR.h"

TypeDecl::TypeDecl() {}

TypeDecl::TypeDecl(int tag, TypeSymbol tsymbol) {}

TypeDecl::TypeDecl(int tag, TypeSymbol tsymbol, int flags) { }

TypeDecl::TypeDecl(int tag, TypeSymbol tsymbol, string name, int flags) { }

TypeDecl::~TypeDecl() { }

void TypeDecl::translate() { }

int TypeDecl::getTypeTag() {
  return tag;
}

string TypeDecl::getTypeDeclName() 
{
  return name;
}
