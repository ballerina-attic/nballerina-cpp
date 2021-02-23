#include "BIR.h"

Param::Param(Location *pos, string namep, int flagsp, TypeDecl *typep)
    : name(namep), flags(flagsp), type(typep) {
  setLocation(pos);
}
