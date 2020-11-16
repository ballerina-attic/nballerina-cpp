#include "BIR.h"

FuncParam::FuncParam() {}

FuncParam::FuncParam(Location *pos, TypeDecl *ty, string name,
                     string metaVarName, VarKind kind, VarScope scope, int off,
                     bool hasDefExp)
    : VarDecl(pos, ty, name, metaVarName, kind, scope, off),
      hasDefaultExpr(hasDefExp) {}

FuncParam::~FuncParam() {}

void FuncParam::translate(LLVMModuleRef &modRef) {}
