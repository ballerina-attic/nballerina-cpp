#include "BIR.h"
#ifndef unix 
    #define __attribute__(unused)
#endif

TypeCastInsn::TypeCastInsn() {}

TypeCastInsn::TypeCastInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *rOp, TypeDecl *tDecl, bool checkTypes)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp(rOp), typeDecl(tDecl),
      checkTypes(checkTypes) {}

TypeCastInsn::~TypeCastInsn() {}

void TypeCastInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  string lhsOpName = getLhsOperand()->name();
  string rhsOpName = rhsOp->name();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  LLVMValueRef rhsOpRef;
  LLVMValueRef lhsOpRef;
  LLVMTypeRef lhsTypeRef;
  if (funcObj) {
    rhsOpRef = funcObj->getLocalVarRefUsingId(rhsOpName);
    lhsOpRef = funcObj->getLocalVarRefUsingId(lhsOpName);
    lhsTypeRef = wrap(unwrap(lhsOpRef)->getType());
    VarDecl *orignamVarDecl = funcObj->getLocalVarDeclFromName(rhsOpName);
    if (orignamVarDecl &&
        orignamVarDecl->getTypeDecl()->getTypeTag() == TYPE_TAG_ANY) {
      LLVMValueRef lastTypeIdx __attribute__((unused)) =
          LLVMBuildStructGEP(builder, rhsOpRef, 1, "lastTypeIdx");

      // TBD: Here, We should be checking whether this type can be cast to
      // lhsType or not by calling a runtime library function in Rust.
      // And the return value of the call should be checked.
      //  If it returns false, then should be a call to abort().

      LLVMValueRef data = LLVMBuildStructGEP(builder, rhsOpRef, 2, "data");

      LLVMValueRef dataLoad = LLVMBuildLoad(builder, data, "");

      LLVMValueRef castResult =
          LLVMBuildBitCast(builder, dataLoad, lhsTypeRef, lhsOpName.c_str());
      LLVMValueRef castLoad = LLVMBuildLoad(builder, castResult, "");
      LLVMBuildStore(builder, castLoad, lhsOpRef);
    } else if (getLhsOperand() && funcObj->getLocalVarDeclFromName(lhsOpName)
                                          ->getTypeDecl()
                                          ->getTypeTag() == TYPE_TAG_ANY) {
      LLVMValueRef structAllocaRef =
          funcObj->getLocalVarRefUsingId(getLhsOperand()->name());
      StringTableBuilder *strTable = getPkgAddress()->getStrTableBuilder();

      // struct first element original type
      LLVMValueRef origTypeIdx =
          LLVMBuildStructGEP(builder, structAllocaRef, 0, "origTypeIdx");
      VarDecl *origVarDecl = funcObj->getLocalVarDeclFromName(lhsOpName);
      assert(origVarDecl->getTypeDecl()->getTypeTag());
      TypeTagEnum origTypeTag =
          TypeTagEnum(origVarDecl->getTypeDecl()->getTypeTag());
      const char *origTypeName = funcObj->getTypeNameOfTypeTag(origTypeTag);
      if (!strTable->contains(origTypeName))
        strTable->add(origTypeName);
      LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), -1, 0);
      LLVMValueRef origStoreRef =
          LLVMBuildStore(builder, constValue, origTypeIdx);
      getPkgAddress()->addStringOffsetRelocationEntry(origTypeName,
                                                      origStoreRef);
      // struct second element last type
      LLVMValueRef lastTypeIdx =
          LLVMBuildStructGEP(builder, structAllocaRef, 1, "lastTypeIdx");
      VarDecl *lastTypeVarDecl = funcObj->getLocalVarDeclFromName(rhsOpName);
      assert(lastTypeVarDecl->getTypeDecl()->getTypeTag());
      TypeTagEnum lastTypeTag =
          TypeTagEnum(lastTypeVarDecl->getTypeDecl()->getTypeTag());
      const char *lastTypeName = funcObj->getTypeNameOfTypeTag(lastTypeTag);
      if (!strTable->contains(lastTypeName))
        strTable->add(lastTypeName);
      LLVMValueRef constValue1 = LLVMConstInt(LLVMInt32Type(), -2, 0);
      LLVMValueRef lastStoreRef =
          LLVMBuildStore(builder, constValue1, lastTypeIdx);
      getPkgAddress()->addStringOffsetRelocationEntry(lastTypeName,
                                                      lastStoreRef);

      // struct third element void pointer data.
      LLVMValueRef elePtr2 =
          LLVMBuildStructGEP(builder, structAllocaRef, 2, "data");
      LLVMValueRef bitCastRes1 = LLVMBuildBitCast(
          builder, rhsOpRef, LLVMPointerType(LLVMInt8Type(), 0), "");
      LLVMBuildStore(builder, bitCastRes1, elePtr2);
    } else {
      LLVMBuildBitCast(builder, rhsOpRef, lhsTypeRef, "data_cast");
    }
  }
}
