#include "BalInvokableType.h"

namespace nballerina {
InvokableType::InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *restTy,
                             TypeDecl *retTy)
    : paramTypes(std::move(paramTy)) {
  restType.reset(restTy);
  returnType.reset(retTy);
}

InvokableType::InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *retTy)
    : paramTypes(std::move(paramTy)) {
  returnType.reset(retTy);
}

const TypeDecl *InvokableType::getReturnType() { return returnType.get(); }
const TypeDecl *InvokableType::getRestType() { return restType.get(); }
TypeDecl *InvokableType::getParamType(int i) { return paramTypes[i]; }
size_t InvokableType::getParamTypeCount() { return paramTypes.size(); }
} // namespace nballerina