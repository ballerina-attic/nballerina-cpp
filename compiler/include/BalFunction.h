#ifndef __BALFUNCTION__H__
#define __BALFUNCTION__H__

#include "BalFuncParam.h"
#include "BalInvokableType.h"
#include "BalParam.h"
#include "BalVarDecl.h"
#include "Debuggable.h"
#include "PackageNode.h"
#include "Translatable.h"
#include <llvm/IR/Function.h>
#include <map>
#include <string>
#include <vector>

// forward declare
class BIRBasicBlock;
class Function;

namespace nballerina {
enum TypeTagEnum {
  TYPE_TAG_INT = 1,
  TYPE_TAG_BYTE = 2,
  TYPE_TAG_FLOAT = 3,
  TYPE_TAG_DECIMAL = 4,
  TYPE_TAG_STRING = 5,
  TYPE_TAG_BOOLEAN = 6,
  TYPE_TAG_JSON = 7,
  TYPE_TAG_XML = 8,
  TYPE_TAG_TABLE = 9,
  TYPE_TAG_NIL = 10,
  TYPE_TAG_ANYDATA = 11,
  TYPE_TAG_RECORD = 12,
  TYPE_TAG_TYPEDESC = 13,
  TYPE_TAG_STREAM = 14,
  TYPE_TAG_MAP = 15,
  TYPE_TAG_INVOKABLE = 16,
  TYPE_TAG_ANY = 17,
  TYPE_TAG_ENDPOINT = 18,
  TYPE_TAG_ARRAY = 19,
  TYPE_TAG_UNION = 20,
  TYPE_TAG_INTERSECTION = 21,
  TYPE_TAG_PACKAGE = 22,
  TYPE_TAG_NONE = 23,
  TYPE_TAG_VOID = 24,
  TYPE_TAG_XMLNS = 25,
  TYPE_TAG_ANNOTATION = 26,
  TYPE_TAG_SEMANTIC_ERROR = 27,
  TYPE_TAG_ERROR = 28,
  TYPE_TAG_ITERATOR = 29,
  TYPE_TAG_TUPLE = 30,
  TYPE_TAG_FUTURE = 31,
  TYPE_TAG_FINITE = 32,
  TYPE_TAG_OBJECT = 33,
  TYPE_TAG_SERVICE = 34,
  TYPE_TAG_BYTE_ARRAY = 35,
  TYPE_TAG_FUNCTION_POINTER = 36,
  TYPE_TAG_HANDLE = 37,
  TYPE_TAG_READONLY = 38,
  TYPE_TAG_SIGNED32_INT = 39,
  TYPE_TAG_SIGNED16_INT = 40,
  TYPE_TAG_SIGNED8_INT = 41,
  TYPE_TAG_UNSIGNED32_INT = 42,
  TYPE_TAG_UNSIGNED16_INT = 43,
  TYPE_TAG_UNSIGNED8_INT = 44,
  TYPE_TAG_CHAR_STRING = 45,
  TYPE_TAG_XML_ELEMENT = 46,
  TYPE_TAG_XML_PI = 47,
  TYPE_TAG_XML_COMMENT = 48,
  TYPE_TAG_XML_TEXT = 49,
  TYPE_TAG_NEVER = 50,
  TYPE_TAG_NULL_SET = 51,
  TYPE_TAG_PARAMETERIZED_TYPE = 52
};

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
  void patchInsn(Function *llvnFun);
  LLVMTypeRef getLLVMFuncRetTypeRefOfType(VarDecl *vDecl);
  VarDecl *getNameVarDecl(std::string opName);
  const char *getTypeNameOfTypeTag(TypeTagEnum typeTag);
  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALFUNCTION__H__