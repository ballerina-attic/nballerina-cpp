#ifndef __BALFUNCTION__H__
#define __BALFUNCTION__H__

#include "BalEnums.h"
#include "Debuggable.h"
#include "PackageNode.h"
#include "Translatable.h"
#include <map>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class BIRBasicBlock;
class FuncParam;
class Operand;
class VarDecl;
class InvokableType;
class Param;
class TypeDecl;

class BIRFunction : public PackageNode, public Debuggable, public Translatable {
private:
  std::string name;
  int flags;
  InvokableType *type;
  std::vector<Operand *> requiredParams;
  VarDecl *receiver;
  Param *restParam;
  int paramCount;
  std::map<std::string, VarDecl *> localVars;
  VarDecl *returnVar;

  std::vector<BIRBasicBlock *> basicBlocks;
  std::string workerName;
  LLVMBuilderRef builder;
  LLVMValueRef newFunction;
  std::map<FuncParam *, std::vector<BIRBasicBlock *>> params;
  std::map<std::string, LLVMValueRef> localVarRefs;
  std::map<std::string, LLVMValueRef> branchComparisonList;

public:
  BIRFunction() = default;
  BIRFunction(Location *pos, std::string pname, int pflags,
              InvokableType *ptype, std::string pworkerName);
  BIRFunction(const BIRFunction &) = delete;
  ~BIRFunction() = default;

  std::string getName() { return name; }
  int getFlags() { return flags; }
  InvokableType *getInvokableType() { return type; }
  std::vector<Operand *> getParams() { return requiredParams; }
  Operand *getParam(int i) { return requiredParams[i]; }
  VarDecl *getReceiver() { return receiver; }
  Param *getRestParam() { return restParam; }

  VarDecl *getReturnVar() { return returnVar; }
  std::vector<BIRBasicBlock *> getBasicBlocks() { return basicBlocks; }
  size_t numBasicBlocks() { return basicBlocks.size(); }
  BIRBasicBlock *getBasicBlock(int i) { return basicBlocks[i]; }
  std::string getWorkerName() { return workerName; }
  LLVMBuilderRef getLLVMBuilder() { return builder; }
  int getNumParams() { return paramCount; }
  LLVMValueRef getNewFunctionRef() { return newFunction; }
  std::map<std::string, LLVMValueRef> getLocalVarRefs() { return localVarRefs; }
  std::map<std::string, LLVMValueRef> getBranchComparisonList() {
    return branchComparisonList;
  }
  LLVMValueRef getValueRefBasedOnName(std::string lhsName);

  void setName(std::string newName) { name = newName; }
  void setFlags(int newFlags) { flags = newFlags; }
  void setInvokableType(InvokableType *t) { type = t; }
  void setParams(std::vector<Operand *> p) { requiredParams = p; }
  void setParam(Operand *param) { requiredParams.push_back(param); }
  void setReceiver(VarDecl *var) { receiver = var; }
  void setRestParam(Param *param) { restParam = param; }
  void setNumParams(int paramcount) { paramCount = paramcount; }
  void setLocalVar(std::string name, VarDecl *var) {
    localVars.insert(std::pair<std::string, VarDecl *>(name, var));
  }
  void setReturnVar(VarDecl *var) { returnVar = var; }
  void setBIRBasicBlocks(std::vector<BIRBasicBlock *> b) { basicBlocks = b; }
  void addBIRBasicBlock(BIRBasicBlock *bb) { basicBlocks.push_back(bb); }
  void setWorkerName(std::string newName) { workerName = newName; }
  void setLLVMBuilder(LLVMBuilderRef b) { builder = b; }
  void setLocalVarRefs(std::map<std::string, LLVMValueRef> newLocalVarRefs) {
    localVarRefs = newLocalVarRefs;
  }
  void setNewFunctionRef(LLVMValueRef newFuncRef) { newFunction = newFuncRef; }

  void setBranchComparisonlist(std::map<std::string, LLVMValueRef> brCompl) {
    branchComparisonList = brCompl;
  }
  void addNewbranchComparison(std::string name, LLVMValueRef compRef) {
    branchComparisonList.insert(
        std::pair<std::string, LLVMValueRef>(name, compRef));
  }

  BIRBasicBlock *searchBb(std::string name);

  LLVMTypeRef getLLVMTypeRefOfType(TypeDecl *typeD);
  LLVMValueRef getLocalVarRefUsingId(std::string locVar);
  LLVMValueRef getLocalToTempVar(Operand *op);
  void translateFunctionBody(LLVMModuleRef &modRef);
  // void patchInsn(llvm::Function *llvnFun);
  LLVMTypeRef getLLVMFuncRetTypeRefOfType(VarDecl *vDecl);
  VarDecl *getNameVarDecl(std::string opName);
  const char *getTypeNameOfTypeTag(TypeTagEnum typeTag);
  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALFUNCTION__H__