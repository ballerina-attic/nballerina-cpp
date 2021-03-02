#include "BalFunctionCallInsn.h"
#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include "llvm-c/Core.h"

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;

namespace nballerina {

FunctionCallInsn::FunctionCallInsn(bool funcVirtual, std::string funcName,
                                   int argNumber, BasicBlock *nextBB,
                                   Operand *lhsOp,
                                   std::vector<Operand *> &fnArgs,
                                   BasicBlock *currentBB)
    : TerminatorInsn(lhsOp, currentBB, nextBB, true), isVirtual(funcVirtual),
      functionName(funcName), argCount(argNumber), argsList(std::move(fnArgs)) {
  kind = INSTRUCTION_KIND_CALL;
}

std::string FunctionCallInsn::getFunctionName() { return functionName; }
int FunctionCallInsn::getArgCount() { return argCount; }
Operand *FunctionCallInsn::getArgumentFromList(int i) { return argsList[i]; }

void FunctionCallInsn::translate(__attribute__((unused))
                                 LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  LLVMValueRef *ParamRefs = new LLVMValueRef[argCount];
  string funName;

  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  Function *birFunc = getPkgAddress()->getFunctionLookUp(functionName);
  assert(birFunc);
  LLVMValueRef namedFuncRef = birFunc->getNewFunctionRef();

  for (int i = 0; i < argCount; i++) {
    Operand *op = argsList[i];
    LLVMValueRef opRef = funcObj->getLocalToTempVar(op);
    ParamRefs[i] = opRef;
  }

  string lhsName = getLhsOperand()->getName();
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

} // namespace nballerina