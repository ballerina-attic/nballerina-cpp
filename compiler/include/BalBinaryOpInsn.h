#ifndef __BALBINARYOPINSN__H__
#define __BALBINARYOPINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declaration
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

  Operand *getRhsOp1();
  Operand *getRhsOp2();
  void setRhsOp1(Operand *op);
  void setRhsOp2(Operand *op);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALBINARYOPINSN__H__