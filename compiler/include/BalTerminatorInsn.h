#ifndef __BALTERMINATORINSN__H__
#define __BALTERMINATORINSN__H__

#include "BalAbstractInsn.h"
#include "interfaces/Translatable.h"

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;

class TerminatorInsn : public AbstractInsn, public Translatable {
private:
  BasicBlock *thenBB;
  bool patchRequire = false;

protected:
  InstructionKind kind;

public:
  TerminatorInsn() = delete;
  TerminatorInsn(Operand *lOp, BasicBlock *currentBB, BasicBlock *then,
                 bool _patchRequire);
  virtual ~TerminatorInsn() = default;

  BasicBlock *getNextBB();
  bool getPatchStatus();
  InstructionKind getInstKind();
  void setPatched();
  void setNextBB(BasicBlock *bb);

  virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__BALTERMINATORINSN__H__