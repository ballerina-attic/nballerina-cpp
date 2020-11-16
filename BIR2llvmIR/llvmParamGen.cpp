#include "BIR.h"

Param::Param() {}

Param::Param(Location *pos, string namep, int flagsp, TypeDecl *typep)
    : BIRNode(pos), name(namep), flags(flagsp), type(typep) {}

Param::~Param() {}

void Param::translate(LLVMModuleRef &modRef) {}
