#ifndef __BALGOTOINSN__H__
#define __BALGOTOINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

class GoToInsn : public TerminatorInsn {
private:
  LLVMValueRef llvmInsn;

public:
  GoToInsn() = default;
  GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
           BIRBasicBlock *nextBB);
  ~GoToInsn() = default;

  LLVMValueRef getLLVMInsn() { return llvmInsn; }
  void setLLVMInsn(LLVMValueRef insn) { llvmInsn = insn; }

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALGOTOINSN__H__