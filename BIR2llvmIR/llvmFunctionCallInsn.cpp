#include "BIR.h"

FunctionCallInsn::FunctionCallInsn() { }

FunctionCallInsn::FunctionCallInsn (string funcName, bool funcVirtual, int argNumber,
                        Operand *lhsOp, BIRBasicBlock *thenBB) {

}

FunctionCallInsn::~FunctionCallInsn() { }

void FunctionCallInsn::translate (LLVMModuleRef &modRef) {
  LLVMValueRef *ParamRefs = new LLVMValueRef[argCount]; 
  LLVMBuilderRef builder;
  
  if (getFunction())
    builder = getFunction()->getLLVMBuilder();
  
  string funName;
  LLVMValueRef namedFuncRef;

  BIRFunction *birFunc = getPkgAddress()->getFunctionLookUp(functionName);
  if (birFunc)
    namedFuncRef = birFunc->getNewFunctionRef(); 
  
  for (int i = 0;  i < argCount;  i++)
  {
    Operand *op = argsList[i];
    LLVMValueRef opRef = getFunction()->getLocalToTempVar(op);
    ParamRefs[i] = opRef;
  }

  string lhsName;
  LLVMValueRef lhsRef;
  if (getLhsOperand() && getLhsOperand()->getVarDecl()) {
    lhsName = getLhsOperand()->getVarDecl()->getVarName();
    if (lhsName != "") {
      lhsRef = getFunction()->getLocalVarRefUsingId(lhsName);
      if (!lhsRef)
        lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsName);
    }
  } 
  int argCount = getArgCount();
  LLVMValueRef callResult;
  if (builder && namedFuncRef) {
    callResult = LLVMBuildCall(builder, namedFuncRef,
                           ParamRefs, argCount, "call");
    if (callResult && lhsRef) {
      LLVMBuildStore(builder, callResult, lhsRef);
    }
  }

    // creating branch to next basic block.
  if (getNextBB() && builder)
    LLVMBuildBr(builder, getNextBB()->getLLVMBBRef());
}
