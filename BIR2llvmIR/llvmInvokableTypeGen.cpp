#include "BIR.h"

InvokableType::InvokableType() {}

InvokableType::InvokableType(vector<TypeDecl> paramTy, TypeDecl restTy, TypeDecl retTy,
                  TypeSymbol tSymbol) {}

InvokableType::InvokableType(vector<TypeDecl> paramTy, TypeDecl retTy, TypeSymbol tSymbol) {}

InvokableType::~InvokableType() {}

TypeDecl InvokableType::getreturnType()
{
  return returnType;
}
