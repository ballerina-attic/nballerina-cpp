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
  Package *parentPackage;
  std::string name;
  std::string workerName;
  int flags;
  InvokableType *type;
  std::vector<FunctionParam *> requiredParams;
  Variable *receiver;
  RestParam *restParam;
  Variable *returnVar;
  std::map<std::string, Variable *> localVars;
  std::map<std::string, BasicBlock *> basicBlocksMap;
  std::vector<BasicBlock *> basicBlocks;
  LLVMBuilderRef llvmBuilder;
  LLVMValueRef llvmFunction;
  std::map<std::string, LLVMValueRef> branchComparisonList;
  std::map<std::string, LLVMValueRef> localVarRefs;

public:
  Function() = delete;
  Function(Package *parentPackage, std::string name, std::string workerName,
           int pflags, InvokableType *ptype);
  Function(const Function &) = delete;
  ~Function() = default;

  FunctionParam *getParam(int index);
  std::string getName();
  size_t getNumParams();
  RestParam *getRestParam();
  Variable *getReturnVar();
  std::vector<BasicBlock *> getBasicBlocks();
  LLVMBuilderRef getLLVMBuilder();
  LLVMValueRef getLLVMFunctionValue();
  LLVMValueRef getLLVMValueForBranchComparison(std::string lhsName);
  LLVMValueRef getLLVMLocalVar(std::string varName);
  BasicBlock *FindBasicBlock(std::string id);

  void insertParam(FunctionParam *param);
  void setRestParam(RestParam *param);
  void setReceiver(Variable *var);
  void setReturnVar(Variable *var);
  void insertLocalVar(Variable *var);
  void insertBasicBlock(BasicBlock *bb);
  void insertBranchComparisonValue(std::string lhsName, LLVMValueRef compRef);
  void setLLVMBuilder(LLVMBuilderRef builder);
  void setLLVMFunctionValue(LLVMValueRef funcRef);

  LLVMValueRef getLocalToTempVar(Operand *op);
  void translateFunctionBody(LLVMModuleRef &modRef);
  // void patchInsn(llvm::Function *llvnFun);
  Variable *getLocalVarFromName(std::string opName);
  LLVMTypeRef getLLVMFuncRetTypeRefOfType(Variable *vDecl);
  Variable *getLocalOrGlobalVariable(Operand *op);
  LLVMValueRef getLocalOrGlobalLLVMValue(Operand *op);
  Package *getPackage() final;
  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALFUNCTION__H__
