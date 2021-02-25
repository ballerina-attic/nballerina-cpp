#ifndef __BALOPERAND__H__
#define __BALOPERAND__H__

#include "interfaces/PackageNode.h"
#include <string>

namespace nballerina {

// Forward Declaration
class VarDecl;

class Operand : public PackageNode {
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