#ifndef __BALTYPETESTINSN__H__
#define __BALTYPETESTINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class TypeDecl;
class Operand;

class TypeTestInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  TypeDecl *typeDecl;

public:
  TypeTestInsn() = default;
  TypeTestInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp,
               TypeDecl *tDecl);
  ~TypeTestInsn() = default;

  Operand *getRhsOp() { return rhsOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void setTypeDecl(TypeDecl *tDecl) { typeDecl = tDecl; }
};

} // namespace nballerina

#endif //!__BALTYPETESTINSN__H__