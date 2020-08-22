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
               string srcFileName)
{
  org            = orgName;
  name           = pkgName;
  version        = verName;
  sourceFileName = srcFileName;
}

void BIRPackage::translate (string modRef) // change to LLVMModuleRef type Arg
{
  BIRFunction *birFunc1 = new BIRFunction();
  
  functions.push_back(birFunc1);
  string ModuleName = org + name + version;
  //LLVMModuleRef mod = LLVMModuleCreateWithName(moduleID);
  for (int i = 0; i < functions.size(); i++)
  {
    BIRFunction *birFunc = functions[i];
    birFunc->translate(ModuleName);  // we have to pass LLVMModuleRef to translate Function.
  }

}

BIRPackage::~BIRPackage()
{
}
