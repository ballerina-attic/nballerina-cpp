#include "BalFuncParam.h"

namespace nballerina {

FuncParam::FuncParam(Location *pos, TypeDecl *ty, std::string name,
                     std::string metaVarName, VarKind kind, VarScope scope,
                     int off, bool hasDefExp)
    : VarDecl(pos, ty, name, metaVarName, kind, scope, off),
      hasDefaultExpr(hasDefExp) {}

} // namespace nballerina
