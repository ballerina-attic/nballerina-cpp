#ifndef __BALINVOKABLETYPE__H__
#define __BALINVOKABLETYPE__H__

#include <vector>

namespace nballerina {

// Forward Declaration
class Type;

class InvokableType {
private:
  std::vector<Type *> paramTypes;
  Type *restType;
  Type *returnType;

public:
  InvokableType() = delete;
  InvokableType(std::vector<Type *> paramTy, Type *restTy,
                Type *retTy);
  InvokableType(std::vector<Type *> paramTy, Type *retTy);
  ~InvokableType() = default;

  const Type *getReturnType();
  const Type *getRestType();
  Type *getParamType(int i);
  size_t getParamTypeCount();
};

} // namespace nballerina

#endif //!__BALINVOKABLETYPE__H__