#include "BIR.h"
#include "BalTypeDecl.h"

InvokableType::InvokableType(vector<TypeDecl *> paramTy, TypeDecl *restTy,
                             TypeDecl *retTy)
    : paramTypes(paramTy), restType(restTy), returnType(retTy) {}

InvokableType::InvokableType(vector<TypeDecl *> paramTy, TypeDecl *retTy)
    : paramTypes(paramTy), returnType(retTy) {}
