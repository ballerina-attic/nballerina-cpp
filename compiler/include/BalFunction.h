#ifndef __BALFUNCTION__H__
#define __BALFUNCTION__H__

#include "BalRestParam.h"
#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"
#include "interfaces/Translatable.h"
#include <map>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class BasicBlock;
class Operand;
class FunctionParam;
class Variable;
class InvokableType;
class RestParam;
class Type;
class Package;

class Function : public PackageNode, public Debuggable, public Translatable {
private:
  std::string name;
  std::string workerName;
  int flags;
  InvokableType *type;

  std::vector<FunctionParam *> requiredParams;
  Variable *receiver;
  RestParam *restParam;
  Variable *returnVar;
  std::map<std::string, Variable *> localVars;
  std::vector<BasicBlock *> basicBlocks;

  LLVMBuilderRef builder;
  LLVMValueRef newFunction;
  Package *parentPackage;
  std::map<std::string, LLVMValueRef> localVarRefs;
  std::map<std::string, LLVMValueRef> branchComparisonList;

public:
  Function() = delete;
  Function(std::string name, std::string workerName, int pflags,
           InvokableType *ptype);
  Function(const Function &) = delete;
  ~Function() = default;

  FunctionParam *getParam(int index);
  BasicBlock *getBasicBlock(int index);
  std::string getName();
  int getFlags();
  size_t getNumParams();
  InvokableType *getInvokableType();
  std::vector<FunctionParam *> getParams();
  Variable *getReceiver();
  RestParam *getRestParam();
  Variable *getReturnVar();
  std::vector<BasicBlock *> getBasicBlocks();
  size_t numBasicBlocks();
  std::string getWorkerName();
  LLVMBuilderRef getLLVMBuilder();
  LLVMValueRef getNewFunctionRef();
  std::map<std::string, LLVMValueRef> getLocalVarRefs();
  std::map<std::string, LLVMValueRef> getBranchComparisonList();
  LLVMValueRef getValueRefBasedOnName(std::string lhsName);

  void insertParam(FunctionParam *param);
  void setRestParam(RestParam *param);
  void setReceiver(Variable *var);
  void setReturnVar(Variable *var);
  void insertLocalVar(Variable *var);
  void insertBasicBlock(BasicBlock *bb);

  void setBasicBlocks(std::vector<BasicBlock *> b);
  void setWorkerName(std::string newName);
  void setLLVMBuilder(LLVMBuilderRef b);
  void setLocalVarRefs(std::map<std::string, LLVMValueRef> newLocalVarRefs);
  void setNewFunctionRef(LLVMValueRef newFuncRef);
  void setBranchComparisonlist(std::map<std::string, LLVMValueRef> brCompl);
  void addNewbranchComparison(std::string name, LLVMValueRef compRef);
  void setPackage(Package *pkg);

  BasicBlock *searchBb(std::string name);

  LLVMValueRef getLocalToTempVar(Operand *op);
  void translateFunctionBody(LLVMModuleRef &modRef);
  // void patchInsn(llvm::Function *llvnFun);
  LLVMValueRef getLocalVarRefUsingId(std::string locVar);
  Variable *getLocalVarFromName(std::string opName);
  LLVMTypeRef getLLVMFuncRetTypeRefOfType(Variable *vDecl);
  Variable *getLocalOrGlobalVariable(Operand *op);
  LLVMValueRef getLocalOrGlobalLLVMValue(Operand *op);
  Package *getPackage() final;
  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALFUNCTION__H__
