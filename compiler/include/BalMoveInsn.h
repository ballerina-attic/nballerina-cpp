#ifndef __BALMOVEINSN__H__
#define __BALMOVEINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

class MoveInsn : public NonTerminatorInsn {
  private:
    Operand *rhsOp;

  public:
    MoveInsn() = delete;
    MoveInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp);
    ~MoveInsn() = default;

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALMOVEINSN__H__
