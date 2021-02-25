#include "BalTypeTestInsn.h"

namespace nballerina {

TypeTestInsn::TypeTestInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp,
                           TypeDecl *tDecl)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp), typeDecl(tDecl) {}

Operand *TypeTestInsn::getRhsOp() { return rhsOp; }
TypeDecl *TypeTestInsn::getTypeDecl() { return typeDecl; }

} // namespace nballerina