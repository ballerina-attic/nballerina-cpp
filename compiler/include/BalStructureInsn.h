#ifndef __BALSTRUCTUREINSN__H__
#define __BALSTRUCTUREINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declaration
class Operand;
class Variable;

class StructureInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  void mapInsnTranslate(Variable *lhsVar, LLVMModuleRef &modRef);
  LLVMValueRef getNewMapIntDeclaration(LLVMModuleRef &modRef, Package *pkg);

public:
  StructureInsn() = delete;
  StructureInsn(Operand *lOp, BasicBlock *currentBB, Operand *rhsOp);
  ~StructureInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALSTRUCTUREINSN__H__
