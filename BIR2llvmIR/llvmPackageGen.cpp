#include "BIR.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>

using namespace std;

BIRPackage::BIRPackage()
{
}

BIRPackage::BIRPackage(string orgName, string pkgName, string verName,
               string srcFileName):
	org(orgName), name(pkgName), version(verName), sourceFileName(srcFileName) {
}

LLVMValueRef BIRPackage::getGlobalVarRefUsingId(string globVar)
{
  map<string, LLVMValueRef>::iterator it;
  
  it = globalVarRefs.find(globVar);
  if(it == globalVarRefs.end()) {
    return NULL;
  }
  else
    return it->second;
   
  return NULL;
}

void BIRPackage::translate (LLVMModuleRef &modRef)
{
  // iterate over all global variables and translate
  for (unsigned int i = 0; i < globalVars.size(); i++)
  {
    LLVMValueRef globVarRef;
    VarDecl* globVar = globalVars[i];
    BIRFunction *funcObj = new BIRFunction();
    LLVMTypeRef varTyperef = funcObj->getLLVMTypeRefOfType (
                                globVar->getTypeDecl());
    string varName = globVar->getVarName();
    if (varTyperef && modRef) {
      // emit/adding the global variable.
      Constant* init_value  = Constant::getNullValue(unwrap(varTyperef));
      GlobalVariable *gVar = new GlobalVariable(*unwrap(modRef), 
		unwrap(varTyperef), false, GlobalValue::ExternalLinkage,
                      init_value, varName.c_str(),0);
      gVar->setAlignment(Align(4));
      globVarRef = wrap(gVar);
    }
    if (globVarRef)
      globalVarRefs.insert({globVar->getVarName(), globVarRef});
  }

  // iterating over each function inside the package.
  for (unsigned int i = 0; i < functions.size(); i++)
  {
    BIRFunction *birFunc = functions[i];
    birFunc->setpkgAddress (this);    
    birFunc->translate(modRef);
  }
}

BIRPackage::~BIRPackage()
{
}
