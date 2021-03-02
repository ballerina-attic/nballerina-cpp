#include "BalInvokableType.h"

namespace nballerina {

InvokableType::InvokableType(std::vector<Type *> paramTy, Type *restTy,
                             Type *retTy)
    : paramTypes(std::move(paramTy)), restType(restTy), returnType(retTy) {}

InvokableType::InvokableType(std::vector<Type *> paramTy, Type *retTy)
    : paramTypes(std::move(paramTy)), returnType(retTy) {}

const Type *InvokableType::getReturnType() { return returnType; }
const Type *InvokableType::getRestType() { return restType; }
Type *InvokableType::getParamType(int i) { return paramTypes[i]; }
size_t InvokableType::getParamTypeCount() { return paramTypes.size(); }

} // namespace nballerina