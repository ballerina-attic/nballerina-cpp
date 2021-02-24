#ifndef __BALMOVEINSN__H__
#define __BALMOVEINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

class MoveInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;

public:
  MoveInsn() = default;
  MoveInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp);
  ~MoveInsn() = default;

  Operand *getRhsOp();
  void setRhsOp(Operand *rOp);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALMOVEINSN__H__