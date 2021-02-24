#include "BalTypeTestInsn.h"

namespace nballerina {

TypeTestInsn::TypeTestInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *rOp, TypeDecl *tDecl)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp), typeDecl(tDecl) {}

}