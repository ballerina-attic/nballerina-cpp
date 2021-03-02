#ifndef __BALTYPECASTINSN__H__
#define __BALTYPECASTINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class Type;
class Operand;

class TypeCastInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  Type *typeDecl;
  bool checkTypes;

public:
  TypeCastInsn() = delete;
  TypeCastInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp,
               Type *tDecl, bool checkTypes);
  ~TypeCastInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALTYPECASTINSN__H__