#ifndef __TRANSLATABLE__H__
#define __TRANSLATABLE__H__

#include <llvm/IR/Type.h>

namespace nballerina {

class Translatable {
public:
  virtual void translate(LLVMModuleRef &modRef) = 0;
};

} // namespace nballerina

#endif //!__TRANSLATABLE__H__