#ifndef __BALOPERAND__H__
#define __BALOPERAND__H__

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
  int typeTag();
};

} // namespace nballerina

#endif //!__BALOPERAND__H__