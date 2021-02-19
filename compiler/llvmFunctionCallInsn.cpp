#include "BIR.h"

void FunctionCallInsn::translate(__attribute__((unused))
                                 LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  LLVMValueRef *ParamRefs = new LLVMValueRef[argCount];
  string funName;

  assert(funcObj);
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  assert(getPkgAddress());

  BIRFunction *birFunc = getPkgAddress()->getFunctionLookUp(functionName);
  assert(birFunc);
  LLVMValueRef namedFuncRef = birFunc->getNewFunctionRef();

  for (int i = 0; i < argCount; i++) {
    Operand *op = argsList[i];
    LLVMValueRef opRef = funcObj->getLocalToTempVar(op);
    ParamRefs[i] = opRef;
  }

  assert(getLhsOperand() && getLhsOperand()->getVarDecl());
  string lhsName = getLhsOperand()->name();
  if (lhsName == "")
    return;

  LLVMValueRef lhsRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsRef)
    lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsName);

  LLVMValueRef callResult;
  if (namedFuncRef) {
    callResult =
        LLVMBuildCall(builder, namedFuncRef, ParamRefs, argCount, "call");
    if (callResult && lhsRef) {
      LLVMBuildStore(builder, callResult, lhsRef);
    }
  }

  // creating branch to next basic block.
  if (getNextBB())
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
}
