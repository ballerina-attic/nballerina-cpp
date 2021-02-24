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

void FunctionCallInsn::setIsVirtual(bool funcVirtual) {
  isVirtual = funcVirtual;
}
void FunctionCallInsn::setFunctionName(std::string funcName) {
  functionName = funcName;
}
void FunctionCallInsn::setArgCount(int argNumber) { argCount = argNumber; }
void FunctionCallInsn::setArgumentsList(std::vector<Operand *> fnArgs) {
  argsList = fnArgs;
}
void FunctionCallInsn::addArgumentToList(Operand *arg) {
  argsList.push_back(arg);
}
void FunctionCallInsn::setRestParam(Param *rParam) { restParam = rParam; }

bool FunctionCallInsn::getIsVirtual() { return isVirtual; }
std::string FunctionCallInsn::getFunctionName() { return functionName; }
int FunctionCallInsn::getArgCount() { return argCount; }
Operand *FunctionCallInsn::getArgumentFromList(int i) { return argsList[i]; }
std::vector<Operand *> FunctionCallInsn::getArgumentsList() { return argsList; }
Param *FunctionCallInsn::getRestParam() { return restParam; }

void FunctionCallInsn::translate(__attribute__((unused))
                                 LLVMModuleRef &modRef) {
  Function *funcObj = getFunction();
  LLVMValueRef *ParamRefs = new LLVMValueRef[argCount];
  string funName;

  assert(funcObj);
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  assert(getPkgAddress());

  Function *birFunc = getPkgAddress()->getFunctionLookUp(functionName);
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

} // namespace nballerina