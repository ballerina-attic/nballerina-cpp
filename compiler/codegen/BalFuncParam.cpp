#include "BalFuncParam.h"

namespace nballerina {

FuncParam::FuncParam(TypeDecl *ty, std::string name, std::string metaVarName,
                     VarKind kind, VarScope scope, int off, bool hasDefExp)
    : VarDecl(ty, name, kind, scope, off), hasDefaultExpr(hasDefExp) {
      setMetaVarName(metaVarName);
    }

bool FuncParam::hasDefaultValue() { return hasDefaultExpr; }

} // namespace nballerina
