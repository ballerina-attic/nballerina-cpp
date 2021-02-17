#include "BIR.h"

void FunctionCallInsn::translate(__attribute__((unused))
                                 LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  LLVMValueRef *ParamRefs = new LLVMValueRef[argCount];
  LLVMBuilderRef builder;
  string funName;
  LLVMValueRef namedFuncRef;
  BIRFunction *birFunc;

  if (funcObj && getPkgAddress()) {
    builder = funcObj->getLLVMBuilder();
    birFunc = getPkgAddress()->getFunctionLookUp(functionName);
    if (birFunc)
      namedFuncRef = birFunc->getNewFunctionRef();
  }

  for (int i = 0; i < argCount; i++) {
    Operand *op = argsList[i];
    LLVMValueRef opRef = funcObj->getLocalToTempVar(op);
    ParamRefs[i] = opRef;
  }

  string lhsName;
  LLVMValueRef lhsRef;
  if (getLhsOperand() && getLhsOperand()->getVarDecl()) {
    lhsName = getLhsOperand()->name();
    if (lhsName != "") {
      lhsRef = funcObj->getLocalVarRefUsingId(lhsName);
      if (!lhsRef)
        lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsName);
    }
  }
  int argCount = getArgCount();
  LLVMValueRef callResult;
  if (builder && namedFuncRef) {
    callResult =
        LLVMBuildCall(builder, namedFuncRef, ParamRefs, argCount, "call");
    if (callResult && lhsRef) {
      LLVMBuildStore(builder, callResult, lhsRef);
    }
  }

  // creating branch to next basic block.
  if (getNextBB() && builder)
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
}
