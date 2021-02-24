#include "BalTypeTestInsn.h"

namespace nballerina {

TypeTestInsn::TypeTestInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *rOp, TypeDecl *tDecl)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp), typeDecl(tDecl) {}

Operand *TypeTestInsn::getRhsOp() { return rhsOp; }
TypeDecl *TypeTestInsn::getTypeDecl() { return typeDecl; }

void TypeTestInsn::setRhsOp(Operand *op) { rhsOp = op; }
void TypeTestInsn::setTypeDecl(TypeDecl *tDecl) { typeDecl = tDecl; }

} // namespace nballerina