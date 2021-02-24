#ifndef __BALMAPSTOREINSN__H__
#define __BALMAPSTOREINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {
class MapStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;
  LLVMValueRef getMapIntStoreDeclaration(LLVMModuleRef &modRef,
                                         Package *pkg);

public:
  MapStoreInsn() = default;
  MapStoreInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *KOp,
               Operand *ROp);
  ~MapStoreInsn() = default;

  void setKeyOp(Operand *kOp);
  void setRhsOp(Operand *rOp);
  Operand *getKeyOp();
  Operand *getRhsOp();

  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALMAPSTOREINSN__H__