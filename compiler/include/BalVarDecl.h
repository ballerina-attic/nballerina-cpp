#ifndef __BALVARDECL__H__
#define __BALVARDECL__H__

#include "BalEnums.h"
#include <string>

namespace nballerina {

// Forward Declaration
class Type;

class VarDecl {
private:
  Type *type;
  std::string varName;
  VarKind kind;

public:
  VarDecl() = delete;
  // For global vars, local  ,arg, return
  VarDecl(Type *ty, std::string name, VarKind k);
  // No type - for function params
  VarDecl(std::string name);
  // Read operad - typedcl could be null
  // VarDecl(Type *ty, std::string name, VarKind k, VarScope sc, bool
  // ignore);
  virtual ~VarDecl() = default;

  Type *getTypeDecl();
  VarKind getVarKind();
  std::string getVarName();

  void setTypeDecl(Type *newType);
  void setVarKind(VarKind newKind);
};

} // namespace nballerina

#endif //!__BALVARDECL__H__