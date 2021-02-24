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

  Operand *getRhsOp();
  TypeDecl *getTypeDecl();

  void setRhsOp(Operand *op);
  void setTypeDecl(TypeDecl *tDecl);
};

} // namespace nballerina

#endif //!__BALTYPETESTINSN__H__