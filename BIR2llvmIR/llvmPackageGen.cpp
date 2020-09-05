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

void BIRPackage::translate (LLVMModuleRef &modRef)
{
  // iterating over each function inside the package.
  for (unsigned int i = 0; i < functions.size(); i++)
  {
    BIRFunction *birFunc = functions[i];
    
    birFunc->translate(modRef);
  }
}

BIRPackage::~BIRPackage()
{
}
