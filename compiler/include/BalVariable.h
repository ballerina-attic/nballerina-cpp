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

class Variable {
private:
  Type *type;
  std::string name;
  VarKind kind;

public:
  Variable() = delete;
  Variable(Type *type, std::string name, VarKind kind);
  virtual ~Variable() = default;

  Type *getTypeDecl();
  VarKind getVarKind();
  std::string getVarName();
};

} // namespace nballerina

#endif //!__BALVARDECL__H__