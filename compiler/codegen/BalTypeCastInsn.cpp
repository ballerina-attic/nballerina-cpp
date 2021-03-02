#include "BalTypeCastInsn.h"
#include "BalEnums.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalType.h"
#include "BalVarDecl.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;
using namespace llvm;

namespace nballerina {

TypeCastInsn::TypeCastInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp,
                           Type *tDecl, bool checkTypes)
    : NonTerminatorInsn(lOp, currentBB), rhsOp(rOp), typeDecl(tDecl),
      checkTypes(checkTypes) {}

void TypeCastInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  string lhsOpName = getLhsOperand()->getName();
  string rhsOpName = rhsOp->getName();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  LLVMValueRef rhsOpRef;
  LLVMValueRef lhsOpRef;
  LLVMTypeRef lhsTypeRef;

  rhsOpRef = funcObj->getLocalVarRefUsingId(rhsOpName);
  lhsOpRef = funcObj->getLocalVarRefUsingId(lhsOpName);
  lhsTypeRef = wrap(unwrap(lhsOpRef)->getType());
  VarDecl *orignamVarDecl = funcObj->getNameVarDecl(rhsOpName);

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
  } else if (getLhsOperand() &&
             funcObj->getNameVarDecl(lhsOpName)->getTypeDecl()->getTypeTag() ==
                 TYPE_TAG_ANY) {
    LLVMValueRef structAllocaRef =
        funcObj->getLocalVarRefUsingId(getLhsOperand()->getName());
    StringTableBuilder *strTable = getPkgAddress()->getStrTableBuilder();

    // struct first element original type
    LLVMValueRef origTypeIdx =
        LLVMBuildStructGEP(builder, structAllocaRef, 0, "origTypeIdx");
    VarDecl *origVarDecl = funcObj->getNameVarDecl(lhsOpName);
    TypeTag origTypeTag =
        origVarDecl->getTypeDecl()->getTypeTag();
    const char *origTypeName = Type::getNameOfType(origTypeTag);
    if (!strTable->contains(origTypeName))
      strTable->add(origTypeName);
    LLVMValueRef constValue = LLVMConstInt(LLVMInt32Type(), -1, 0);
    LLVMValueRef origStoreRef =
        LLVMBuildStore(builder, constValue, origTypeIdx);
    getPkgAddress()->addStringOffsetRelocationEntry(origTypeName, origStoreRef);
    // struct second element last type
    LLVMValueRef lastTypeIdx =
        LLVMBuildStructGEP(builder, structAllocaRef, 1, "lastTypeIdx");
    VarDecl *lastTypeVarDecl = funcObj->getNameVarDecl(rhsOpName);
    assert(lastTypeVarDecl->getTypeDecl()->getTypeTag());
    TypeTag lastTypeTag =
        lastTypeVarDecl->getTypeDecl()->getTypeTag();
    const char *lastTypeName = Type::getNameOfType(lastTypeTag);
    if (!strTable->contains(lastTypeName))
      strTable->add(lastTypeName);
    LLVMValueRef constValue1 = LLVMConstInt(LLVMInt32Type(), -2, 0);
    LLVMValueRef lastStoreRef =
        LLVMBuildStore(builder, constValue1, lastTypeIdx);
    getPkgAddress()->addStringOffsetRelocationEntry(lastTypeName, lastStoreRef);

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

} // namespace nballerina