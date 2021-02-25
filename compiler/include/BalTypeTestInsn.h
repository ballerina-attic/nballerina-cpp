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
  TypeTestInsn() = delete;
  TypeTestInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp, TypeDecl *tDecl);
  ~TypeTestInsn() = default;

  Operand *getRhsOp();
  TypeDecl *getTypeDecl();
};

} // namespace nballerina

#endif //!__BALTYPETESTINSN__H__