#include "BalInvokableType.h"

namespace nballerina {

InvokableType::InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *restTy,
                             TypeDecl *retTy)
    : paramTypes(std::move(paramTy)), restType(restTy), returnType(retTy) {}

InvokableType::InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *retTy)
    : paramTypes(std::move(paramTy)), returnType(retTy) {}

const TypeDecl *InvokableType::getReturnType() { return returnType; }
const TypeDecl *InvokableType::getRestType() { return restType; }
TypeDecl *InvokableType::getParamType(int i) { return paramTypes[i]; }
size_t InvokableType::getParamTypeCount() { return paramTypes.size(); }

} // namespace nballerina