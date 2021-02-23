#include "BIR.h"

VarDecl::VarDecl(Location *loc, string pname, string pmetaVarName)
    : varName(pname), metaVarName(pmetaVarName) {
  setLocation(loc);
}

VarDecl::VarDecl(Location *loc, TypeDecl *ty, string name, string metaName,
                 VarKind k, VarScope sc, int offset)
    : type(ty), varName(name), metaVarName(metaName), kind(k), scope(sc),
      insOffset(offset) {
  setLocation(loc);
}
