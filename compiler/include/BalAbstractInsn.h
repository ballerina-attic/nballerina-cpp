#ifndef __BALABSTRACTINSN__H__
#define __BALABSTRACTINSN__H__

#include "interfaces/Debuggable.h"

namespace nballerina {

// Forward Declaration
class Operand;
class Function;
class BasicBlock;
class Package;

class AbstractInsn : public Debuggable {
private:
  Operand *lhsOp;
  BasicBlock *currentBB;

protected:
  Operand *getLhsOperand();
  Function *getFunction();
  Package *getPkgAddress();

public:
  AbstractInsn() = delete;
  AbstractInsn(Operand *lOp, BasicBlock *currentBB);
  virtual ~AbstractInsn() = default;

};

} // namespace nballerina

#endif //!__BALABSTRACTINSN__H__