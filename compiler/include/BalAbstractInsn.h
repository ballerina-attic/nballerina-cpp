#ifndef __BALABSTRACTINSN__H__
#define __BALABSTRACTINSN__H__

#include "BalEnums.h"
#include "interfaces/Debuggable.h"

namespace nballerina {

// Forward Declaration
class Operand;
class Function;
class BasicBlock;
class Package;

class AbstractInsn : public Debuggable {
private:
  InstructionKind kind;
  Operand *lhsOp;
  BasicBlock *currentBB;

public:
  AbstractInsn() = default;
  AbstractInsn(Location *pos, InstructionKind kind, Operand *lOp);
  virtual ~AbstractInsn() = default;

  InstructionKind getInstKind();
  Operand *getLhsOperand();
  Function *getFunction();
  BasicBlock *getCurrentBB();
  Package *getPkgAddress();

  void setInstKind(InstructionKind newKind);
  void setLhsOperand(Operand *lOp);
  void setCurrentBB(BasicBlock *currB);
};

} // namespace nballerina

#endif //!__BALABSTRACTINSN__H__