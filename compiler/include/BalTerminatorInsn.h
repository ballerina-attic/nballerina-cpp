#ifndef __BALTERMINATORINSN__H__
#define __BALTERMINATORINSN__H__

#include "BalAbstractInsn.h"
#include "BalEnums.h"
#include "Translatable.h"

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;

class TerminatorInsn : public AbstractInsn, public Translatable {
private:
  BasicBlock *thenBB;
  bool patchRequire;

public:
  TerminatorInsn() = default;
  TerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 BasicBlock *then);
  virtual ~TerminatorInsn() = default;

  BasicBlock *getNextBB();
  bool getPatchStatus();

  void setNextBB(BasicBlock *block);
  void setPatchStatus(bool patchrequire);
  virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__BALTERMINATORINSN__H__