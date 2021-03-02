#include "BalPackage.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalType.h"
#include "BalVariable.h"
#include "llvm-c/Core.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"

using namespace std;

namespace nballerina {

Package::Package(string orgName, string pkgName, string verName,
                 string srcFileName)
    : org(orgName), name(pkgName), version(verName),
      sourceFileName(srcFileName) {}

// return ValueRef of global variable based on variable name.
LLVMValueRef Package::getGlobalVarRefUsingId(string globVar) {
  map<string, LLVMValueRef>::iterator it;
  it = globalVarRefs.find(globVar);
  if (it == globalVarRefs.end()) {
    return NULL;
  } else {
    return it->second;
  }
}

std::string Package::getOrgName() { return org; }
std::string Package::getPackageName() { return name; }
std::string Package::getVersion() { return version; }
std::string Package::getSrcFileName() { return sourceFileName; }
llvm::StringTableBuilder *Package::getStrTableBuilder() { return strBuilder; }
void Package::setOrgName(std::string orgName) { org = orgName; }
void Package::setPackageName(std::string pkgName) { name = pkgName; }
void Package::setVersion(std::string verName) { version = verName; }
void Package::setSrcFileName(std::string srcFileName) {
  sourceFileName = srcFileName;
}
std::vector<Function *> Package::getFunctions() { return functions; }
Function *Package::getFunction(int i) { return functions[i]; }
std::map<std::string, LLVMValueRef> Package::getGlobalVarRefs() {
  return globalVarRefs;
}
llvm::StructType *Package::getStructType() { return structType; }
void Package::setFunctions(std::vector<Function *> f) { functions = f; }
void Package::addFunction(Function *f) { functions.push_back(f); }
void Package::addFunctionLookUpEntry(std::string funcName,
                                     Function *BIRfunction) {
  functionLookUp.insert(
      std::pair<std::string, Function *>(funcName, BIRfunction));
}
Function *Package::getFunctionLookUp(std::string funcName) {
  return functionLookUp.at(funcName);
}
size_t Package::getNumFunctions() { return functions.size(); }
void Package::addArrayFunctionRef(std::string arrayName,
                                  LLVMValueRef functionRef) {
  arrayFunctionRefs.insert(
      std::pair<std::string, LLVMValueRef>(arrayName, functionRef));
}
std::map<std::string, LLVMValueRef> Package::getArrayFuncRefMap() {
  return arrayFunctionRefs;
}

void Package::translate(LLVMModuleRef &modRef) {
  // String Table initialization
  strBuilder = new llvm::StringTableBuilder(llvm::StringTableBuilder::RAW, 1);
  // iterate over all global variables and translate
  for (auto const it : globalVars) {
    LLVMValueRef globVarRef;
    Variable *globVar = it.second;
    Function *funcObj = new Function();
    LLVMTypeRef varTyperef =
        funcObj->getLLVMTypeRefOfType(globVar->getTypeDecl());
    string varName = globVar->getVarName();
    if (varTyperef && modRef) {
      // emit/adding the global variable.
      llvm::Constant *initValue =
          llvm::Constant::getNullValue(llvm::unwrap(varTyperef));
      llvm::GlobalVariable *gVar = new llvm::GlobalVariable(
          *llvm::unwrap(modRef), llvm::unwrap(varTyperef), false,
          llvm::GlobalValue::ExternalLinkage, initValue, varName.c_str(), 0);
      gVar->setAlignment(llvm::Align(4));
      globVarRef = wrap(gVar);
      if (globVarRef)
        globalVarRefs.insert({globVar->getVarName(), globVarRef});
    }
  }

  // creating struct smart pointer to store any type variables data.
  LLVMTypeRef structGen =
      LLVMStructCreateNamed(LLVMGetGlobalContext(), "struct.smtPtr");
  LLVMTypeRef *structElementTypes = new LLVMTypeRef[3];
  structElementTypes[0] = LLVMInt32Type();
  structElementTypes[1] = LLVMInt32Type();
  structElementTypes[2] = LLVMPointerType(LLVMInt8Type(), 0);
  LLVMStructSetBody(structGen, structElementTypes, 3, 0);
  structType = llvm::unwrap<llvm::StructType>(structGen);

  // iterating over each function, first create function definition
  // (without function body) and adding to Module.
  map<string, Function *>::iterator it;
  for (it = functionLookUp.begin(); it != functionLookUp.end(); it++) {
    Function *birFunc = it->second;
    LLVMBuilderRef builder = LLVMCreateBuilder();
    birFunc->setLLVMBuilder(builder);
    LLVMTypeRef funcType;
    unsigned int numParams = birFunc->getNumParams();
    LLVMTypeRef *paramTypes = new LLVMTypeRef[numParams];
    bool isVarArg = false;
    birFunc->setPkgAddress(this);
    if (birFunc->getRestParam())
      isVarArg = true;

    if (!birFunc->getReturnVar())
      continue;
    LLVMTypeRef retType =
        birFunc->getLLVMFuncRetTypeRefOfType(birFunc->getReturnVar());
    if (!retType)
      continue;

    for (unsigned i = 0; i < numParams; i++) {
      Operand *funcParam = birFunc->getParam(i);
      assert(funcParam->getType());
      paramTypes[i] = birFunc->getLLVMTypeRefOfType(funcParam->getType());
    }

    funcType = LLVMFunctionType(retType, paramTypes, numParams, isVarArg);
    if (funcType) {
      birFunc->setNewFunctionRef(
          LLVMAddFunction(modRef, birFunc->getName().c_str(), funcType));
    }
  }

  // iterating over each function translate the function body.
  map<string, Function *>::iterator it1;
  for (it1 = functionLookUp.begin(); it1 != functionLookUp.end(); it1++) {
    Function *birFunc = it1->second;
    birFunc->setPkgAddress(this);
    birFunc->translate(modRef);
  }
  // This Api will finalize the string table builder if table size is not
  // zero.
  if (strBuilder->getSize() != 0)
    applyStringOffsetRelocations(modRef);
}

void Package::addStringOffsetRelocationEntry(string eleType,
                                             LLVMValueRef storeInsn) {
  if (structElementStoreInst.size() == 0) {
    vector<LLVMValueRef> temp;
    temp.push_back(storeInsn);
    structElementStoreInst.insert(
        pair<string, vector<LLVMValueRef>>(eleType, temp));
  } else {
    map<string, vector<LLVMValueRef>>::iterator itr;
    itr = structElementStoreInst.find(eleType);
    if (itr == structElementStoreInst.end()) {
      vector<LLVMValueRef> temp1;
      temp1.push_back(storeInsn);
      structElementStoreInst.insert(
          pair<string, vector<LLVMValueRef>>(eleType, temp1));
    } else {
      vector<LLVMValueRef> temp2 = itr->second;
      temp2.push_back(storeInsn);
      itr->second = temp2;
    }
  }
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void Package::applyStringOffsetRelocations(__attribute__((unused))
                                           LLVMModuleRef &modRef) {
  strBuilder->finalize();
  map<string, vector<LLVMValueRef>>::iterator itr;
  for (itr = structElementStoreInst.begin();
       itr != structElementStoreInst.end(); itr++) {
    size_t finalOrigOffset = strBuilder->getOffset(itr->first);
    vector<LLVMValueRef> temp = itr->second;
    LLVMValueRef tempVal = LLVMConstInt(LLVMInt32Type(), finalOrigOffset, 0);
    for (unsigned int i = 0; i < temp.size(); i++) {
      LLVMValueRef storeInsn = temp[i];
      LLVMValueRef constOperand = LLVMGetOperand(storeInsn, 0);
      LLVMReplaceAllUsesWith(constOperand, tempVal);
      break;
    }
  }
}

LLVMValueRef Package::getFunctionRefBasedOnName(string arrayName) {
  map<string, LLVMValueRef>::iterator it;
  it = arrayFunctionRefs.find(arrayName);

  if (it == arrayFunctionRefs.end()) {
    return NULL;
  } else
    return it->second;
}

Variable *Package::getGlobalVarDeclFromName(string name) {
  auto varIt = globalVars.find(name);
  if (varIt == globalVars.end())
    return nullptr;

  return varIt->second;
}

void Package::insertGlobalVar(Variable *g) {
  globalVars.insert(std::pair<std::string, Variable *>(g->getVarName(), g));
}

} // namespace nballerina