#ifndef __BALOPERAND__H__
#define __BALOPERAND__H__

#include "BalVariable.h"
#include <string>

namespace nballerina {

class Operand {
private:
  std::string name;
  VarKind kind;

public:
  Operand() = delete;
  Operand(std::string name, VarKind kind);
  virtual ~Operand() = default;

  std::string getName();
  VarKind getKind();
};

class FunctionParam : public Operand {
private:
  Type *type;

public:
  FunctionParam() = delete;
  FunctionParam(std::string name);
  ~FunctionParam() = default;

  Type *getType();
  void setType(Type *type);
};

} // namespace nballerina

#endif //!__BALOPERAND__H__