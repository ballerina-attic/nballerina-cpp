#ifndef __BALBINARYOPINSN__H__
#define __BALBINARYOPINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class Operand;

class BinaryOpInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp1;
  Operand *rhsOp2;

public:
  BinaryOpInsn() = default;
  BinaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp1,
               Operand *rOp2);
  ~BinaryOpInsn() = default;

  Operand *getRhsOp1() { return rhsOp1; }
  Operand *getRhsOp2() { return rhsOp2; }
  void setRhsOp1(Operand *op) { rhsOp1 = op; }
  void setRhsOp2(Operand *op) { rhsOp2 = op; }

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALBINARYOPINSN__H__