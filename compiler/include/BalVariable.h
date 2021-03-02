#ifndef __BALVARDECL__H__
#define __BALVARDECL__H__

#include "interfaces/VariableKind.h"
#include <string>

namespace nballerina {

// Forward Declaration
class Type;

class Variable : public VariableKind {
private:
  Type *type;

public:
  Variable() = delete;
  Variable(Type *type, std::string name, VarKind kind);
  virtual ~Variable() = default;

  Type *getTypeDecl();
};

} // namespace nballerina

#endif //!__BALVARDECL__H__
