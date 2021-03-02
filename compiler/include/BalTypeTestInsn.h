#ifndef __BALTYPETESTINSN__H__
#define __BALTYPETESTINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class Type;
class Operand;

class TypeTestInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  Type *typeDecl;

public:
  TypeTestInsn() = delete;
  TypeTestInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp,
               Type *tDecl);
  ~TypeTestInsn() = default;
};

} // namespace nballerina

#endif //!__BALTYPETESTINSN__H__
