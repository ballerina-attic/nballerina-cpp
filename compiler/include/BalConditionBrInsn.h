#ifndef __BALCONDITIONBRINSN__H__
#define __BALCONDITIONBRINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

class ConditionBrInsn : public TerminatorInsn {
private:
  BasicBlock *ifThenBB;
  BasicBlock *elseBB;

public:
  ConditionBrInsn() = default;
  ConditionBrInsn(Location *pos, InstructionKind kind, Operand *lOp,
                  BasicBlock *nextB);
  ~ConditionBrInsn() = default;
  void setIfThenBB(BasicBlock *ifBB) { ifThenBB = ifBB; }
  void setElseBB(BasicBlock *elseB) { elseBB = elseB; }

  BasicBlock *getIfThenBB() { return ifThenBB; }
  BasicBlock *getElseBB() { return elseBB; }

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONDITIONBRINSN__H__