#ifndef __BALOPERAND__H__
#define __BALOPERAND__H__

#include "PackageNode.h"
#include <string>

namespace nballerina {

// Forward Declaration
class VarDecl;

class Operand : public PackageNode {
private:
  VarDecl *varDecl;

public:
  Operand() = default;
  Operand(VarDecl *vDecl);
  ~Operand() = default;

  VarDecl *getVarDecl();
  void setVarDecl(VarDecl *newDecl);
  std::string name();
  int typeTag();
};

} // namespace nballerina

#endif //!__BALOPERAND__H__