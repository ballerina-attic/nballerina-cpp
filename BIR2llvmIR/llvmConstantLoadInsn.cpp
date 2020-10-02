#include "BIR.h"

ConstantLoadInsn::ConstantLoadInsn () {
  value = 0;
}

ConstantLoadInsn::ConstantLoadInsn (Location *pos, InstructionKind kind,
			Operand *lOp, unsigned long long val):
		NonTerminatorInsn(pos, kind, lOp), value(val) {

}

ConstantLoadInsn::~ConstantLoadInsn() {
}

void ConstantLoadInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;   
  LLVMValueRef   lhsRef;
  LLVMValueRef   constRef;
  if (getLhsOperand() && 
	getLhsOperand()->getVarDecl()) {
    builder = getFunction()->getLLVMBuilder();
    if (getpkgAddress() && 
	  getLhsOperand()->getVarDecl()->getVarKind() == GLOBAL_VAR_KIND) {
      lhsRef = getpkgAddress()->getGlobalVarRefUsingId(
                        getLhsOperand()->getVarDecl()->getVarName());
    }
    else if (getFunction() && 
		(getLhsOperand()->getVarDecl()->getVarKind() == LOCAL_VAR_KIND ||
                getLhsOperand()->getVarDecl()->getVarKind() == TEMP_VAR_KIND ||
		getLhsOperand()->getVarDecl()->getVarKind() == RETURN_VAR_KIND ||
		getLhsOperand()->getVarDecl()->getVarKind() == SELF_VAR_KIND ||
		getLhsOperand()->getVarDecl()->getVarKind() == CONSTANT_VAR_KIND)) {
      lhsRef  = getFunction()->getLocalVarRefUsingId(
                            getLhsOperand()->getVarDecl()->getVarName());
    }
  }
  

  if (getLhsOperand() && getLhsOperand()->getVarDecl() &&
       getLhsOperand()->getVarDecl()->getTypeDecl()) {
    int typeTag = getLhsOperand()->getVarDecl()->getTypeDecl()->getTypeTag();
    switch (typeTag) {
      case TYPE_TAG_ENUM_TYPE_TAG_INT:
      {
        constRef = LLVMConstInt(LLVMInt32Type(), value, 0);
	break;
      }
      case TYPE_TAG_ENUM_TYPE_TAG_BYTE:
      case TYPE_TAG_ENUM_TYPE_TAG_FLOAT:
      case TYPE_TAG_ENUM_TYPE_TAG_BOOLEAN:
      {
        constRef = LLVMConstInt(LLVMInt8Type(), value, 0);
	break;
      }
      default:
        constRef = LLVMConstInt(LLVMInt32Type(), value, 0);
    }
  }
  if (builder && constRef && lhsRef)
    LLVMBuildStore(builder, constRef, lhsRef);
}
