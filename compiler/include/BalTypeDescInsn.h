#ifndef __BALTYPEDESCINSN__H__
#define __BALTYPEDESCINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

class TypeDescInsn : public NonTerminatorInsn {
public:
  TypeDescInsn(Operand *lOp, BasicBlock *currentBB)
      : NonTerminatorInsn(lOp, currentBB){};
  ~TypeDescInsn() = default;
};

} // namespace nballerina

#endif //!__BALTYPEDESCINSN__H__
