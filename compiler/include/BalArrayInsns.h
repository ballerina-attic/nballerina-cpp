#ifndef __BALARRAYINSN__H__
#define __BALARRAYINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward Declaration
class TypeDecl;

class ArrayInsn : public NonTerminatorInsn {
private:
  Operand *sizeOp;
  TypeDecl *typeDecl;
  LLVMValueRef getNewArrayDeclaration(LLVMModuleRef &modRef, Package *pkg);

public:
  ArrayInsn() = delete;
  ArrayInsn(Operand *lOp, BasicBlock *currentBB, Operand *sOp, TypeDecl *TDecl);
  ~ArrayInsn() = default;

  Operand *getSizeOp();
  TypeDecl *getTypeDecl();

  void translate(LLVMModuleRef &modRef) final;
};

class ArrayLoadInsn : public NonTerminatorInsn {
private:
  bool optionalFieldAccess;
  bool fillingRead;
  Operand *keyOp;
  Operand *rhsOp;

public:
  ArrayLoadInsn() = delete;
  ArrayLoadInsn(Operand *lOp, BasicBlock *currentBB, bool opFA, bool fR,
                Operand *KOp, Operand *ROp);
  ~ArrayLoadInsn() = default;

  bool getOptionalFieldAccess();
  bool getFillingRead();
  Operand *getKeyOp();
  Operand *getRhsOp();

  LLVMValueRef getArrayLoadDeclaration(LLVMModuleRef &modRef, Package *pkg);
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

  Operand *getKeyOp();
  Operand *getRhsOp();

  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALARRAYINSN__H__