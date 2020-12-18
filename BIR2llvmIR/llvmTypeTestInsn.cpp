#include "BIR.h"

TypeTestInsn::TypeTestInsn() {}

TypeTestInsn::TypeTestInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *rOp, TypeDecl *tDecl)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp), typeDecl(tDecl) {}

TypeTestInsn::~TypeTestInsn() {}

void TypeTestInsn::translate(LLVMModuleRef &modRef) {}
