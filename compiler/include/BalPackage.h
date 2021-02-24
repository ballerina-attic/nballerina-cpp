#ifndef __BALPACKAGE__H__
#define __BALPACKAGE__H__

#include "interfaces/Translatable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/MC/StringTableBuilder.h"
#include <map>
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class Function;
class VarDecl;

class Package : public Translatable {
private:
  std::string org;
  std::string name;
  std::string version;
  std::string sourceFileName;
  std::vector<Function *> functions;
  std::vector<VarDecl *> globalVars;
  std::map<std::string, LLVMValueRef> globalVarRefs;
  std::map<std::string, Function *> functionLookUp;
  llvm::StructType *structType;
  llvm::StringTableBuilder *strBuilder;
  std::map<std::string, std::vector<LLVMValueRef>> structElementStoreInst;
  std::map<std::string, LLVMValueRef> arrayFunctionRefs;

public:
  Package() = default;
  Package(std::string orgName, std::string pkgName, std::string verName,
             std::string srcFileName);
  ~Package() = default;

  std::string getOrgName();
  std::string getPackageName();
  std::string getVersion();
  std::string getSrcFileName();
  llvm::StringTableBuilder *getStrTableBuilder();
  std::vector<Function *> getFunctions();
  Function *getFunction(int i);
  std::vector<VarDecl *> getGlobalVars();
  std::map<std::string, LLVMValueRef> getGlobalVarRefs();
  llvm::StructType *getStructType();
  size_t getNumFunctions();
  LLVMValueRef getFunctionRefBasedOnName(std::string arrayName);

  void setOrgName(std::string orgName);
  void setPackageName(std::string pkgName);
  void setVersion(std::string verName);
  void setSrcFileName(std::string srcFileName);
  void setFunctions(std::vector<Function *> f);
  void addFunction(Function *f);
  void addGlobalVar(VarDecl *g);
  void addFunctionLookUpEntry(std::string funcName, Function *BIRfunction);
  Function *getFunctionLookUp(std::string funcName);
  void addArrayFunctionRef(std::string arrayName, LLVMValueRef functionRef);
  std::map<std::string, LLVMValueRef> getArrayFuncRefMap();
  LLVMValueRef getGlobalVarRefUsingId(std::string globVar);
  void addStringOffsetRelocationEntry(std::string eleType,
                                      LLVMValueRef storeInsn);

  void applyStringOffsetRelocations(LLVMModuleRef &modRef);
  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALPACKAGE__H__