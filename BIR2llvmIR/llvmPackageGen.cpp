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
  string ModuleName = org + name + version;
  char * cstr = new char [ModuleName.length()+1];
  LLVMModuleRef mod = LLVMModuleCreateWithName(cstr);

  // iterating through each function from current package.
  for (unsigned int i = 0; i < functions.size(); i++)
  {
    BIRFunction *birFunc = functions[i];
    birFunc->translate(mod);
  }
}

BIRPackage::~BIRPackage()
{
}
