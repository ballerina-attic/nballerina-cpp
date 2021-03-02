#ifndef __BALVARDECL__H__
#define __BALVARDECL__H__

#include <string>

namespace nballerina {

// Forward Declaration
class Type;

enum VarKind {
  LOCAL_VAR_KIND = 1,
  ARG_VAR_KIND = 2,
  TEMP_VAR_KIND = 3,
  RETURN_VAR_KIND = 4,
  GLOBAL_VAR_KIND = 5,
  SELF_VAR_KIND = 6,
  CONSTANT_VAR_KIND = 7
};

class VarDecl {
private:
  Type *type;
  std::string name;
  VarKind kind;

public:
  VarDecl() = delete;
  VarDecl(Type *type, std::string name, VarKind kind);

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