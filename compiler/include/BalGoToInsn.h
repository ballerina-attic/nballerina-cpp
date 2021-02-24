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
           BasicBlock *nextBB);
  ~GoToInsn() = default;

  LLVMValueRef getLLVMInsn();
  void setLLVMInsn(LLVMValueRef insn);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALGOTOINSN__H__