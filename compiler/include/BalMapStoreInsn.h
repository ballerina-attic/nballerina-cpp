#ifndef __BALMAPSTOREINSN__H__
#define __BALMAPSTOREINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina
{
    class MapStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;
  LLVMValueRef getMapIntStoreDeclaration(LLVMModuleRef &modRef,
                                         BIRPackage *pkg);

public:
  MapStoreInsn() = default;
  MapStoreInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *KOp,
               Operand *ROp);
  void setKeyOp(Operand *kOp) { keyOp = kOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }
  Operand *getKeyOp() { return keyOp; }
  Operand *getRhsOp() { return rhsOp; }
  ~MapStoreInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif  //!__BALMAPSTOREINSN__H__