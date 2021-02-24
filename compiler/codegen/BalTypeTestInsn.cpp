#include "BalTypeTestInsn.h"

namespace nballerina {

TypeTestInsn::TypeTestInsn(Operand *lOp, Operand *rOp, TypeDecl *tDecl)
    : NonTerminatorInsn(lOp), rhsOp(rOp), typeDecl(tDecl) {}

Operand *TypeTestInsn::getRhsOp() { return rhsOp; }
TypeDecl *TypeTestInsn::getTypeDecl() { return typeDecl; }

} // namespace nballerina