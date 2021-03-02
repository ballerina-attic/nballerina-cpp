#ifndef __BALOPERAND__H__
#define __BALOPERAND__H__

#include "BalType.h"
#include <string>

namespace nballerina {

// Forward Declaration
class VarDecl;

class Operand {
private:
  VarDecl *varDecl;

public:
  Operand() = delete;
  Operand(VarDecl *vDecl);
  ~Operand() = default;

  VarDecl *getVarDecl();
  std::string getName();
  TypeTag typeTag();
};

} // namespace nballerina

#endif //!__BALOPERAND__H__