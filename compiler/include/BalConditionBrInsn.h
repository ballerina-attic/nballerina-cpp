#ifndef __BALCONDITIONBRINSN__H__
#define __BALCONDITIONBRINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

class ConditionBrInsn : public TerminatorInsn {
private:
  BasicBlock *ifThenBB;
  BasicBlock *elseBB;

public:
  ConditionBrInsn() = delete;
  ConditionBrInsn(Operand *lOp, BasicBlock *_ifThenBB, BasicBlock *_elseBB);
  ~ConditionBrInsn() = default;

  BasicBlock *getIfThenBB();
  BasicBlock *getElseBB();

  void setIfThenBB(BasicBlock *bb);
  void setElseBB(BasicBlock *bb);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONDITIONBRINSN__H__