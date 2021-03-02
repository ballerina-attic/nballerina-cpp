#ifndef __BALARRAYINSN__H__
#define __BALARRAYINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declaration
class Type;

class ArrayInsn : public NonTerminatorInsn {
private:
  Operand *sizeOp;
  Type *typeDecl;
  LLVMValueRef getNewArrayDeclaration(LLVMModuleRef &modRef, Package *pkg);

public:
  ArrayInsn() = delete;
  ArrayInsn(Operand *lOp, BasicBlock *currentBB, Operand *sOp, Type *TDecl);
  ~ArrayInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
};

class ArrayLoadInsn : public NonTerminatorInsn {
private:
  bool optionalFieldAccess;
  bool fillingRead;
  Operand *keyOp;
  Operand *rhsOp;
  LLVMValueRef getArrayLoadDeclaration(LLVMModuleRef &modRef, Package *pkg);

public:
  ArrayLoadInsn() = delete;
  ArrayLoadInsn(Operand *lOp, BasicBlock *currentBB, bool opFA, bool fR,
                Operand *KOp, Operand *ROp);
  ~ArrayLoadInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
};

class ArrayStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;
  LLVMValueRef getArrayStoreDeclaration(LLVMModuleRef &modRef, Package *pkg);

public:
  ArrayStoreInsn() = delete;
  ArrayStoreInsn(Operand *lOp, BasicBlock *currentBB, Operand *KOp,
                 Operand *ROp);
  ~ArrayStoreInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALARRAYINSN__H__
