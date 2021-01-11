#include "BIR.h"

ConstantLoadInsn::ConstantLoadInsn() {}

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, int intval)
     :NonTerminatorInsn(pos, kind, lOp), val(intval) {}

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, float fval)
     :NonTerminatorInsn(pos, kind, lOp), val(fval) {}

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, bool boolval)
     :NonTerminatorInsn(pos, kind, lOp), val(boolval) {}

ConstantLoadInsn::ConstantLoadInsn(Location *pos, InstructionKind kind,
                                   Operand *lOp, string *strval)
     :NonTerminatorInsn(pos, kind, lOp), val(strval) {}

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
    TypeTagEnum lhsTypeTag = TypeTagEnum(lhsOp->typeTag());
    assert(lhsTypeTag == typeTag);
    switch (lhsTypeTag) {
    case TYPE_TAG_INT: {
      constRef = LLVMConstInt(LLVMInt32Type(), val.intValue, 0);
      break;
    }
    case TYPE_TAG_FLOAT: {
      constRef = LLVMConstReal(LLVMFloatType(), val.floatValue);
      break;
    }
    case TYPE_TAG_BOOLEAN: {
      constRef = LLVMConstInt(LLVMInt8Type(), val.boolValue, 0);
      break;
    }
    case TYPE_TAG_STRING:
    case TYPE_TAG_CHAR_STRING: {
      LLVMValueRef *paramTypes = new LLVMValueRef[3];
      string stringValue = val.strValue->data();
      Constant *C =
        llvm::ConstantDataArray::getString(*unwrap(LLVMGetGlobalContext()),
                StringRef(stringValue.c_str(), stringValue.length()));
      GlobalVariable *GV = new GlobalVariable(*unwrap(modRef), C->getType(), false,
                                      GlobalValue::PrivateLinkage, C, ".str");
      GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
      GV->setAlignment(llvm::Align(1));

      paramTypes[0] = wrap(unwrap(builder)->getInt64(0));
      paramTypes[1] = wrap(unwrap(builder)->getInt64(0));
      constRef = LLVMBuildInBoundsGEP(builder, wrap(GV),
                              paramTypes, 2, "simple");
      break;
    }
    case TYPE_TAG_NIL: {
      return;
    }
    default:
      llvm_unreachable("Unknown Type");
    }
  }
  if (builder && constRef && lhsRef)
    LLVMBuildStore(builder, constRef, lhsRef);
}
