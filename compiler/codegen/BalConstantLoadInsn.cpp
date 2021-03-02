#include "BalConstantLoad.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "BalVarDecl.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace std;
using namespace llvm;

namespace nballerina {

ConstantLoadInsn::ConstantLoadInsn(Operand *lOp, BasicBlock *currentBB)
    : NonTerminatorInsn(lOp, currentBB) {}

int ConstantLoadInsn::getIntValue() { return val.intValue; }
float ConstantLoadInsn::getFloatValue() { return val.floatValue; }
bool ConstantLoadInsn::getBoolValue() { return val.boolValue; }
std::string *ConstantLoadInsn::getStringValue() { return val.strValue; }

void ConstantLoadInsn::setIntValue(int intVal, TypeTag TypeTag) {
  val.intValue = intVal;
  typeTag = TypeTag;
}
void ConstantLoadInsn::setFloatValue(float floatVal, TypeTag TypeTag) {
  val.floatValue = floatVal;
  typeTag = TypeTag;
}
void ConstantLoadInsn::setBoolValue(bool boolVal, TypeTag TypeTag) {
  val.boolValue = boolVal;
  typeTag = TypeTag;
}
void ConstantLoadInsn::setStringValue(std::string *str, TypeTag TypeTag) {
  val.strValue = str;
  typeTag = TypeTag;
}
// With Nil Type setting only Type Tag because value will be zero with NIL
// Type.
void ConstantLoadInsn::setTypeTagNil(TypeTag TypeTag) { typeTag = TypeTag; }

TypeTag ConstantLoadInsn::getTypeTag() { return typeTag; }

void ConstantLoadInsn::translate(LLVMModuleRef &modRef) {
  LLVMValueRef lhsRef;
  LLVMValueRef constRef;
  Operand *lhsOp = getLhsOperand();
  VarKind varKind = lhsOp->getKind();
  LLVMBuilderRef builder = getFunction()->getLLVMBuilder();
  string lhsVarName = lhsOp->getName();
  VarDecl *lhsVar = nullptr;

  if (varKind == GLOBAL_VAR_KIND) {
    lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsVarName);
    lhsVar = getPkgAddress()->getGlobalVarDeclFromName(lhsVarName);
  } else if ((varKind == LOCAL_VAR_KIND || varKind == TEMP_VAR_KIND ||
              varKind == RETURN_VAR_KIND || varKind == SELF_VAR_KIND ||
              varKind == CONSTANT_VAR_KIND)) {
    lhsRef = getFunction()->getLocalVarRefUsingId(lhsVarName);
    lhsVar = getFunction()->getLocalVarFromName(lhsVarName);
  } else
    llvm_unreachable("Unknown Type");

  TypeTag lhsTypeTag = lhsVar->getTypeDecl()->getTypeTag();
  
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
    Constant *C = llvm::ConstantDataArray::getString(
        *unwrap(LLVMGetGlobalContext()),
        StringRef(stringValue.c_str(), stringValue.length()));
    GlobalVariable *GV =
        new GlobalVariable(*unwrap(modRef), C->getType(), false,
                           GlobalValue::PrivateLinkage, C, ".str");
    GV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
    GV->setAlignment(llvm::Align(1));

    paramTypes[0] = wrap(unwrap(builder)->getInt64(0)); //
    paramTypes[1] = wrap(unwrap(builder)->getInt64(0));
    constRef = LLVMBuildInBoundsGEP(builder, wrap(GV), paramTypes, 2, "simple");
    break;
  }
  case TYPE_TAG_NIL: {
    return;
  }
  default:
    llvm_unreachable("Unknown Type");
  }

  if (constRef && lhsRef)
    LLVMBuildStore(builder, constRef, lhsRef);
}

} // namespace nballerina