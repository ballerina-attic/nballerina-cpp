#include "BIR.h"

ConstantLoadInsn::ConstantLoadInsn() { value = 0; }

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, int val)
    : NonTerminatorInsn(pos, kind, lOp), value(val) {}

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, std::string val)
    : NonTerminatorInsn(pos, kind, lOp), strValue(val) {}

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, float val)
    : NonTerminatorInsn(pos, kind, lOp), floatValue(val) {}

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, bool val)
    : NonTerminatorInsn(pos, kind, lOp), boolValue(val) {}

ConstantLoadInsn::~ConstantLoadInsn() {}

void ConstantLoadInsn::translate(LLVMModuleRef &modRef) {
  LLVMBuilderRef builder;
  LLVMValueRef lhsRef;
  LLVMValueRef constRef;
  Operand *lhsOp = getLhsOperand();
  VarKind varKind = lhsOp->getVarDecl()->getVarKind();

  if (lhsOp && lhsOp->getVarDecl()) {
    builder = getFunction()->getLLVMBuilder();
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
      // Currently, there is no floating point support in nballerina.
      llvm_unreachable("No floating point support in nballerina");
      break;
    }
    case TYPE_TAG_BOOLEAN: {
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
