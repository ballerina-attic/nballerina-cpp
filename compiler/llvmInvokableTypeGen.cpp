#include "BIR.h"
#include "BalTypeDecl.h"

InvokableType::InvokableType(vector<TypeDecl *> paramTy, TypeDecl *restTy,
                             TypeDecl *retTy)
    : paramTypes(paramTy), restType(restTy), returnType(retTy) {}

InvokableType::InvokableType(vector<TypeDecl *> paramTy, TypeDecl *retTy)
    : paramTypes(paramTy), returnType(retTy) {}

TypeDecl *InvokableType::getReturnType() { return returnType; }
TypeDecl *InvokableType::getRestType() { return restType; }
TypeDecl *InvokableType::getParamType(int i) { return paramTypes[i]; }
size_t InvokableType::getParamTypeCount() { return paramTypes.size(); }
