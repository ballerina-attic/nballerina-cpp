#ifndef __BALCONDITIONBRINSN__H__
#define __BALCONDITIONBRINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

class ConditionBrInsn : public TerminatorInsn {
private:
  BIRBasicBlock *ifThenBB;
  BIRBasicBlock *elseBB;

public:
  ConditionBrInsn() = default;
  ConditionBrInsn(Location *pos, InstructionKind kind, Operand *lOp,
                  BIRBasicBlock *nextB);
  ~ConditionBrInsn() = default;
  void setIfThenBB(BIRBasicBlock *ifBB) { ifThenBB = ifBB; }
  void setElseBB(BIRBasicBlock *elseB) { elseBB = elseB; }

  BIRBasicBlock *getIfThenBB() { return ifThenBB; }
  BIRBasicBlock *getElseBB() { return elseBB; }

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONDITIONBRINSN__H__