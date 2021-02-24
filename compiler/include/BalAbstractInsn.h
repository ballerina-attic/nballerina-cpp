#ifndef __BALABSTRACTINSN__H__
#define __BALABSTRACTINSN__H__

#include "BalEnums.h"
#include "Debuggable.h"
#include "PackageNode.h"

namespace nballerina {

// Forward Declaration
class Operand;
class BIRFunction;
class BIRBasicBlock;

class AbstractInsn : public PackageNode, public Debuggable {
private:
  InstructionKind kind;
  Operand *lhsOp;
  BIRFunction *bFunc;
  BIRBasicBlock *currentBB;

public:
  AbstractInsn() = default;
  AbstractInsn(Location *pos, InstructionKind kind, Operand *lOp);
  virtual ~AbstractInsn() = default;

  InstructionKind getInstKind();
  Operand *getLhsOperand();
  BIRFunction *getFunction();
  BIRBasicBlock *getCurrentBB();

  void setFunction(BIRFunction *func);
  void setInstKind(InstructionKind newKind);
  void setLhsOperand(Operand *lOp);
  void setCurrentBB(BIRBasicBlock *currB);
};

} // namespace nballerina

#endif //!__BALABSTRACTINSN__H__