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

public:
  ArrayInsn() = default;
  ArrayInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *sOp,
            TypeDecl *TDecl);
  ~ArrayInsn() = default;

  void setSizeOp(Operand *Size);
  void setTypeDecl(TypeDecl *Type);

  Operand *getSizeOp();
  TypeDecl *getTypeDecl();

  LLVMValueRef getNewArrayDeclaration(LLVMModuleRef &modRef, Package *pkg);
  void translate(LLVMModuleRef &modRef) final;
};

class ArrayLoadInsn : public NonTerminatorInsn {
private:
  bool optionalFieldAccess;
  bool fillingRead;
  Operand *keyOp;
  Operand *rhsOp;

public:
  ArrayLoadInsn() = default;
  ArrayLoadInsn(Location *pos, InstructionKind kind, Operand *lOp, bool opFA,
                bool fR, Operand *KOp, Operand *ROp);
  ~ArrayLoadInsn() = default;

  void setOptionalFieldAccess(bool OpFAccess);
  void setFillingRead(bool fRead);
  void setKeyOp(Operand *kOp);
  void setRhsOp(Operand *rOp);

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

public:
  ArrayStoreInsn() = default;
  ArrayStoreInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 Operand *KOp, Operand *ROp);
  ~ArrayStoreInsn() = default;

  void setKeyOp(Operand *kOp);
  void setRhsOp(Operand *rOp);

  Operand *getKeyOp();
  Operand *getRhsOp();

  LLVMValueRef getArrayStoreDeclaration(LLVMModuleRef &modRef, Package *pkg);
  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALARRAYINSN__H__