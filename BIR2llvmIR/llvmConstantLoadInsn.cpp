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
  if (getFunction() && getLhsOperand() && 
	getLhsOperand()->getVarDecl()) {
    builder = getFunction()->getLLVMBuilder();

    lhsRef  = getFunction()->getLocalVarRefUsingId(
                            getLhsOperand()->getVarDecl()->getVarName());
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

  LLVMBuildStore(builder, constRef, lhsRef);
}
