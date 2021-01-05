#include "BIR.h"

ConstantLoadInsn::ConstantLoadInsn() { value = 0; }

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, int val)
    : NonTerminatorInsn(pos, kind, lOp), value(val) {}

ConstantLoadInsn::~ConstantLoadInsn() {}

void ConstantLoadInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  LLVMValueRef lhsRef;
  LLVMValueRef constRef;
  Operand *lhsOp = getLhsOperand();
  VarKind varKind = lhsOp->getVarDecl()->getVarKind();
  builder = getFunction()->getLLVMBuilder();
  if (lhsOp && lhsOp->getVarDecl()) {
    if (getPkgAddress() &&
        lhsOp->getVarDecl()->getVarKind() == GLOBAL_VAR_KIND) {
      lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsOp->name());
    } else if (getFunction() &&
               (varKind == LOCAL_VAR_KIND || varKind == TEMP_VAR_KIND ||
                varKind == RETURN_VAR_KIND || varKind == SELF_VAR_KIND ||
                varKind == CONSTANT_VAR_KIND)) {
      lhsRef = getFunction()->getLocalVarRefUsingId(lhsOp->name());
    }
  }

  if (lhsOp && lhsOp->getVarDecl() && lhsOp->getVarDecl()->getTypeDecl()) {
    int typeTag = lhsOp->typeTag();
    switch (typeTag) {
    case TYPE_TAG_INT: {
      constRef = LLVMConstInt(LLVMInt32Type(), value, 0);
      break;
    }
    case TYPE_TAG_BYTE:
    case TYPE_TAG_FLOAT: {
      constRef = LLVMConstReal(LLVMFloatType(), floatValue);
      break;
    }
    case TYPE_TAG_BOOLEAN: {
      constRef = LLVMConstInt(LLVMInt8Type(), boolValue, 0);
      break;
    }
    default:
      constRef = LLVMConstInt(LLVMInt32Type(), value, 0);
    }
  }
  if (builder && constRef && lhsRef)
    LLVMBuildStore(builder, constRef, lhsRef);
}
