#ifndef __BALSTRUCTUREINSN__H__
#define __BALSTRUCTUREINSN__H__

#include "BalNonTerminatorInsn.h"
#include "BalVarDecl.h"

namespace nballerina {

class StructureInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  void mapInsnTranslate(VarDecl *lhsVar, LLVMModuleRef &modRef);
  LLVMValueRef getNewMapIntDeclaration(LLVMModuleRef &modRef, Package *pkg);

public:
  StructureInsn() = default;
  ~StructureInsn() = default;
  Operand *getRhsOp() { return rhsOp; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALSTRUCTUREINSN__H__