#include "BIR.h"

VarDecl::VarDecl(Location *loc, string pname, string pmetaVarName)
    : BIRNode(loc), varName(pname), metaVarName(pmetaVarName) {}

VarDecl::VarDecl(Location *loc, TypeDecl *ty, string name, string metaName,
                 VarKind k, VarScope sc, int offset)
    : BIRNode(loc), type(ty), varName(name), metaVarName(metaName), kind(k),
      scope(sc), insOffset(offset) {}
