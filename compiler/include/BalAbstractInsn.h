#ifndef __BALABSTRACTINSN__H__
#define __BALABSTRACTINSN__H__

#include "BalEnums.h"
#include "Debuggable.h"
#include "PackageNode.h"

namespace nballerina {

// Forward Declaration
class Operand;
class Function;
class BasicBlock;

class AbstractInsn : public PackageNode, public Debuggable {
private:
  InstructionKind kind;
  Operand *lhsOp;
  Function *bFunc;
  BasicBlock *currentBB;

public:
  AbstractInsn() = default;
  AbstractInsn(Location *pos, InstructionKind kind, Operand *lOp);
  virtual ~AbstractInsn() = default;

  InstructionKind getInstKind();
  Operand *getLhsOperand();
  Function *getFunction();
  BasicBlock *getCurrentBB();

  void setFunction(Function *func);
  void setInstKind(InstructionKind newKind);
  void setLhsOperand(Operand *lOp);
  void setCurrentBB(BasicBlock *currB);
};

} // namespace nballerina

#endif //!__BALABSTRACTINSN__H__