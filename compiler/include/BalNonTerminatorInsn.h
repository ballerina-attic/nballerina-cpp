#ifndef __BALNONTERMINATORINSN__H__
#define __BALNONTERMINATORINSN__H__

#include "BalAbstractInsn.h"
#include "interfaces/Translatable.h"

namespace nballerina {

// Forward Declaration
class Operand;

class NonTerminatorInsn : public AbstractInsn, public Translatable {
private:
public:
  NonTerminatorInsn() = delete;
  NonTerminatorInsn(Operand *lOp, BasicBlock *currentBB);
  virtual ~NonTerminatorInsn() = default;

  virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__BALNONTERMINATORINSN__H__
