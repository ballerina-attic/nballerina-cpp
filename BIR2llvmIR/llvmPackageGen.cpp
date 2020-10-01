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
  for (unsigned int i = 0; i < GlobalVars.size(); i++)
  {
    LLVMValueRef globVarRef;
    VarDecl* globVar = GlobalVars[i];
    BIRFunction *funcObj = new BIRFunction();
    LLVMTypeRef varTyperef = funcObj->getLLVMTypeRefOfType (
                                globVar->getTypeDecl());
    string varName = globVar->getVarName();
    if (varTyperef && modRef)
      globVarRef = LLVMAddGlobal(modRef, varTyperef, varName.c_str());

    //wrap(new GlobalVariable(*unwrap(modRef), unwrap(varTyperef), false,
     //                            GlobalValue::AppendingLinkage, nullptr, varName.c_str()));
    //unwrap(modRef)->getOrInsertGlobal("__new_global", unwrap(varTyperef));
    LLVMGetNamedGlobal(modRef,varName.c_str());
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
