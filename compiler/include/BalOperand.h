#ifndef __BALOPERAND__H__
#define __BALOPERAND__H__

#include "BalVarDecl.h"
#include <string>

namespace nballerina {

class Operand {
private:
  std::string name;
  VarKind kind;
  Type *type;

public:
  Operand() = delete;
  Operand(std::string name, VarKind kind);
  ~Operand() = default;

  std::string getName();
  VarKind getKind();
  Type *getType();
  void setType(Type *type);
};

} // namespace nballerina

#endif //!__BALOPERAND__H__