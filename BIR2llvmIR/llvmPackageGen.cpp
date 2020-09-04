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

void BIRPackage::translate ()
{
  bool dumpllvm = true; //temp value
  string ModuleName = org + name + version;
  LLVMModuleRef mod = LLVMModuleCreateWithName(ModuleName.c_str());

  // iterating through each function from current package.
  for (unsigned int i = 0; i < functions.size(); i++)
  {
    char* Message = "My Message";
    BIRFunction *birFunc = functions[i];
    
    birFunc->translate(mod);

    // print the LLVMIR using BIR.
    if(dumpllvm) {
      string targetFineName = sourceFileName+".ll";
      LLVMPrintModuleToFile(mod, targetFineName.c_str(), &Message);
    }
  }
}

BIRPackage::~BIRPackage()
{
}
