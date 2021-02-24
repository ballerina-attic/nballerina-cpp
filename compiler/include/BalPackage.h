#ifndef __BALPACKAGE__H__
#define __BALPACKAGE__H__

#include "Translatable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/MC/StringTableBuilder.h"
#include <map>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class BIRFunction;
class VarDecl;

class BIRPackage : public Translatable {
private:
  std::string org;
  std::string name;
  std::string version;
  std::string sourceFileName;
  std::vector<BIRFunction *> functions;
  std::vector<VarDecl *> globalVars;
  std::map<std::string, LLVMValueRef> globalVarRefs;
  std::map<std::string, BIRFunction *> functionLookUp;
  llvm::StructType *structType;
  llvm::StringTableBuilder *strBuilder;
  std::map<std::string, std::vector<LLVMValueRef>> structElementStoreInst;
  std::map<std::string, LLVMValueRef> arrayFunctionRefs;

public:
  BIRPackage() = default;
  BIRPackage(std::string orgName, std::string pkgName, std::string verName,
             std::string srcFileName);
  ~BIRPackage() = default;

  std::string getOrgName() { return org; }
  std::string getPackageName() { return name; }
  std::string getVersion() { return version; }
  std::string getSrcFileName() { return sourceFileName; }
  llvm::StringTableBuilder *getStrTableBuilder() { return strBuilder; }
  void setOrgName(std::string orgName) { org = orgName; }
  void setPackageName(std::string pkgName) { name = pkgName; }
  void setVersion(std::string verName) { version = verName; }
  void setSrcFileName(std::string srcFileName) { sourceFileName = srcFileName; }
  std::vector<BIRFunction *> getFunctions() { return functions; }
  BIRFunction *getFunction(int i) { return functions[i]; }
  std::vector<VarDecl *> getGlobalVars() { return globalVars; }
  std::map<std::string, LLVMValueRef> getGlobalVarRefs() {
    return globalVarRefs;
  }
  llvm::StructType *getStructType() { return structType; }
  void setFunctions(std::vector<BIRFunction *> f) { functions = f; }
  void addFunction(BIRFunction *f) { functions.push_back(f); }
  void addGlobalVar(VarDecl *g) { globalVars.push_back(g); }

  void addFunctionLookUpEntry(std::string funcName, BIRFunction *BIRfunction) {
    functionLookUp.insert(
        std::pair<std::string, BIRFunction *>(funcName, BIRfunction));
  }
  BIRFunction *getFunctionLookUp(std::string funcName) {
    return functionLookUp.at(funcName);
  }
  size_t numFunctions() { return functions.size(); }
  LLVMValueRef getGlobalVarRefUsingId(std::string globVar);
  void addStringOffsetRelocationEntry(std::string eleType,
                                      LLVMValueRef storeInsn);
  void applyStringOffsetRelocations(LLVMModuleRef &modRef);
  void addArrayFunctionRef(std::string arrayName, LLVMValueRef functionRef) {
    arrayFunctionRefs.insert(
        std::pair<std::string, LLVMValueRef>(arrayName, functionRef));
  }
  LLVMValueRef getFunctionRefBasedOnName(std::string arrayName);
  std::map<std::string, LLVMValueRef> getArrayFuncRefMap() {
    return arrayFunctionRefs;
  }
  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALPACKAGE__H__