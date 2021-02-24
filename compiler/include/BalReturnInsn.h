#ifndef __BALRETURNINSN__H__
#define __BALRETURNINSN__H__

#include "BalTerminatorInsn.h"

namespace nballerina {

class ReturnInsn : public TerminatorInsn {
private:
public:
  ReturnInsn();
  ~ReturnInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALRETURNINSN__H__