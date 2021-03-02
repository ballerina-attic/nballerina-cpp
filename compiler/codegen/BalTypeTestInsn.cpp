#include "BalTypeTestInsn.h"

namespace nballerina {

TypeTestInsn::TypeTestInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp,
                           Type *tDecl)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp), typeDecl(tDecl) {}

} // namespace nballerina