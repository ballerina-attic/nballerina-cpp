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

  void setIfThenBB(BasicBlock *ifBB);
  void setElseBB(BasicBlock *elseB);

  BasicBlock *getIfThenBB();
  BasicBlock *getElseBB();

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONDITIONBRINSN__H__