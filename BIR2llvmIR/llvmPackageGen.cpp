#include "BIR.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>

using namespace std;

BIRPackage::BIRPackage() {}

BIRPackage::BIRPackage(string orgName, string pkgName, string verName,
		string srcFileName): org(orgName), name(pkgName), 
		version(verName), sourceFileName(srcFileName) {}

// return ValueRef of global variable based on variable name.
LLVMValueRef BIRPackage::getGlobalVarRefUsingId(string globVar) {
  map<string, LLVMValueRef>::iterator it;
  it = globalVarRefs.find(globVar);
  if (it == globalVarRefs.end()) {
    return NULL;
  }
  else {
    return it->second;
  }
  return NULL;
}

void BIRPackage::translate (LLVMModuleRef &modRef) {
  // iterate over all global variables and translate
  for (unsigned int i = 0; i < globalVars.size(); i++) {
    LLVMValueRef globVarRef;
    VarDecl* globVar = globalVars[i];
    BIRFunction *funcObj = new BIRFunction();
    LLVMTypeRef varTyperef = funcObj->getLLVMTypeRefOfType (
                                globVar->getTypeDecl());
    string varName = globVar->getVarName();
    if (varTyperef && modRef) {
      // emit/adding the global variable.
      Constant* initValue  = Constant::getNullValue(unwrap(varTyperef));
      GlobalVariable *gVar = new GlobalVariable(*unwrap(modRef), 
		unwrap(varTyperef), false, GlobalValue::ExternalLinkage,
                      initValue, varName.c_str(),0);
      gVar->setAlignment(Align(4));
      globVarRef = wrap(gVar);
    }
    if (globVarRef)
      globalVarRefs.insert({globVar->getVarName(), globVarRef});
  }

// iterating over each function, first create function definition 
// (without function body) and adding to Module.
  map<string, BIRFunction *>::iterator it;
  for (it = functionLookUp.begin(); it != functionLookUp.end(); it++) {
    BIRFunction *birFunc = it->second;
    LLVMBuilderRef builder = LLVMCreateBuilder();
    birFunc->setLLVMBuilder(builder);
    LLVMTypeRef   funcType;
    LLVMTypeRef   retType;
    unsigned int  numParams = birFunc->getNumParams();
    LLVMTypeRef  *paramTypes = new LLVMTypeRef[numParams];
    bool          isVarArg = false;
    birFunc->setPkgAddress (this);
    if (birFunc->getRestParam())
      isVarArg = true;

    if (birFunc->getReturnVar())
      retType = birFunc->getLLVMFuncRetTypeRefOfType(birFunc->getReturnVar());
    for (unsigned i = 0;  i < numParams;  i++) {
      Operand *funcParam = birFunc->getParam(i);
      if (funcParam) {
	paramTypes[i] = birFunc->getLLVMTypeRefOfType(funcParam->
				getVarDecl()->getTypeDecl());
      }
    }
    if (retType) {
      funcType = LLVMFunctionType(retType, paramTypes, numParams, isVarArg);
      const char *newFuncName = birFunc->getName().c_str();
      if (funcType) {
        birFunc->setNewFunctionRef(LLVMAddFunction(modRef, newFuncName, funcType));
      }
    }
  }

  // iterating over each function translate the function body.
  map<string, BIRFunction *>::iterator it1;
  for (it1 = functionLookUp.begin(); it1 != functionLookUp.end(); it1++) {
    BIRFunction *birFunc = it1->second;
    birFunc->setPkgAddress (this);    
    birFunc->translate(modRef);
  }
}

BIRPackage::~BIRPackage() {}
