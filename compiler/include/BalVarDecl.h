#ifndef __BALVARDECL__H__
#define __BALVARDECL__H__

#include "BalEnums.h"
#include <string>

namespace nballerina {

// Forward Declaration
class TypeDecl;

class VarDecl {
private:
  TypeDecl *type;
  std::string varName;
  VarKind kind;

public:
  VarDecl() = delete;
  // For global vars, local  ,arg, return
  VarDecl(TypeDecl *ty, std::string name, VarKind k);
  // No type - for function params
  VarDecl(std::string name);
  // Read operad - typedcl could be null
  // VarDecl(TypeDecl *ty, std::string name, VarKind k, VarScope sc, bool
  // ignore);
  virtual ~VarDecl() = default;

  TypeDecl *getTypeDecl();
  VarKind getVarKind();
  std::string getVarName();

  void setTypeDecl(TypeDecl *newType);
  void setVarKind(VarKind newKind);
};

} // namespace nballerina

#endif //!__BALVARDECL__H__