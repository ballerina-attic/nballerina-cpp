#ifndef __BALMAPSTOREINSN__H__
#define __BALMAPSTOREINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {
class MapStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;
  LLVMValueRef getMapIntStoreDeclaration(LLVMModuleRef &modRef, Package *pkg);

public:
  MapStoreInsn() = delete;
  MapStoreInsn(Operand *lOp, BasicBlock *currentBB, Operand *KOp, Operand *ROp);
  ~MapStoreInsn() = default;

  Operand *getKeyOp();
  Operand *getRhsOp();

  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALMAPSTOREINSN__H__