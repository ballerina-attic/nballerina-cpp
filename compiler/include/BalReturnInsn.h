#ifndef __BALRETURNINSN__H__
#define __BALRETURNINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

// Forward Declaration
class BasicBlock;

class ReturnInsn : public TerminatorInsn {
public:
  ReturnInsn(BasicBlock *currentBB);
  ~ReturnInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALRETURNINSN__H__