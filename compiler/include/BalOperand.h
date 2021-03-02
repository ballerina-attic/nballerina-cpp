#ifndef __BALOPERAND__H__
#define __BALOPERAND__H__

#include "interfaces/VariableKind.h"
#include <string>

namespace nballerina {

// Forward Declaration
class Type;

class Operand : public VariableKind {
public:
  Operand() = delete;
  Operand(std::string name, VarKind kind);
  virtual ~Operand() = default;
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