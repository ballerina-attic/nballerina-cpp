#ifndef __BALINVOKABLETYPE__H__
#define __BALINVOKABLETYPE__H__

#include "BalTypeDecl.h"
#include <vector>

namespace nballerina {
class InvokableType {
private:
  std::vector<TypeDecl *> paramTypes;
  const TypeDecl *restType;
  const TypeDecl *returnType;

public:
  InvokableType() = delete;
  InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *restTy,
                TypeDecl *retTy);
  InvokableType(std::vector<TypeDecl *> paramTy, TypeDecl *retTy);
  ~InvokableType() = default;

  const TypeDecl *getReturnType();
  const TypeDecl *getRestType();
  TypeDecl *getParamType(int i);
  size_t getParamTypeCount();
};
} // namespace nballerina

#endif //!__BALINVOKABLETYPE__H__