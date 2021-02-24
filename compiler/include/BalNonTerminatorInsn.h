#ifndef __BALNONTERMINATORINSN__H__
#define __BALNONTERMINATORINSN__H__

#include "BalAbstractInsn.h"
#include "BalEnums.h"
#include "interfaces/Translatable.h"

namespace nballerina {

// Forward Declaration
class Operand;

class NonTerminatorInsn : public AbstractInsn, public Translatable {
private:
public:
  NonTerminatorInsn() = default;
  NonTerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp);
  virtual ~NonTerminatorInsn() = default;

  virtual void translate(LLVMModuleRef &modRef) override;
};

} // namespace nballerina

#endif //!__BALNONTERMINATORINSN__H__