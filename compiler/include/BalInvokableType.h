#ifndef __BALINVOKABLETYPE__H__
#define __BALINVOKABLETYPE__H__

#include <vector>

namespace nballerina {

// forward declaration
class TypeDecl;

class InvokableType {
private:
  std::vector<TypeDecl *> paramTypes;
  TypeDecl *restType;
  TypeDecl *returnType;

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