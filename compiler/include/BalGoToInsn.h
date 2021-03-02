#ifndef __BALGOTOINSN__H__
#define __BALGOTOINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

class GoToInsn : public TerminatorInsn {
public:
  GoToInsn() = delete;
  GoToInsn(BasicBlock *nextBB, BasicBlock *currentBB);
  ~GoToInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALGOTOINSN__H__
