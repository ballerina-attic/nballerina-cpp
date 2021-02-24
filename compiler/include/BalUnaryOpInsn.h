#ifndef __BALUNARYOPINSN__H__
#define __BALUNARYOPINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declare
class Operand;

class UnaryOpInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;

public:
  UnaryOpInsn() = delete;
  UnaryOpInsn(Operand *lOp, Operand *rOp);
  ~UnaryOpInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALUNARYOPINSN__H__