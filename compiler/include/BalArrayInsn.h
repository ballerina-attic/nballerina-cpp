#ifndef __BALARRAYINSN__H__
#define __BALARRAYINSN__H__

#include "BalNonTerminatorInsn.h"

namespace nballerina {

// Forward declarations
class TypeDecl;

class ArrayInsn : public NonTerminatorInsn {
private:
  Operand *sizeOp;
  TypeDecl *typeDecl;

public:
  ArrayInsn() = default;
  ArrayInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *sOp,
            TypeDecl *TDecl);
  void setSizeOp(Operand *Size) { sizeOp = Size; }
  void setTypeDecl(TypeDecl *Type) { typeDecl = Type; }

  Operand *getSizeOp() { return sizeOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  void translate(LLVMModuleRef &modRef) final;
  LLVMValueRef getNewArrayDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
  ~ArrayInsn() = default;
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
  void setOptionalFieldAccess(bool OpFAccess) {
    optionalFieldAccess = OpFAccess;
  }
  void setFillingRead(bool fRead) { fillingRead = fRead; }
  void setKeyOp(Operand *kOp) { keyOp = kOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }
  bool getOptionalFieldAccess() { return optionalFieldAccess; }
  bool getFillingRead() { return fillingRead; }
  Operand *getKeyOp() { return keyOp; }
  Operand *getRhsOp() { return rhsOp; }
  ~ArrayLoadInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
  LLVMValueRef getArrayLoadDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
};

class ArrayStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;

public:
  ArrayStoreInsn() = default;
  ArrayStoreInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 Operand *KOp, Operand *ROp);
  void setKeyOp(Operand *kOp) { keyOp = kOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }
  Operand *getKeyOp() { return keyOp; }
  Operand *getRhsOp() { return rhsOp; }
  ~ArrayStoreInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
  LLVMValueRef getArrayStoreDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
};
} // namespace nballerina

#endif //!__BALARRAYINSN__H__