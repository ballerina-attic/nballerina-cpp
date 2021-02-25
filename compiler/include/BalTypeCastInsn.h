#ifndef __BALTYPECASTINSN__H__
#define __BALTYPECASTINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class TypeDecl;
class Operand;

class TypeCastInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  TypeDecl *typeDecl;
  bool checkTypes;

public:
  TypeCastInsn() = delete;
  TypeCastInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp,
               TypeDecl *tDecl, bool checkTypes);
  ~TypeCastInsn() = default;

  Operand *getRhsOp();
  TypeDecl *getTypeDecl();
  bool mustCheckTypes();

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALTYPECASTINSN__H__