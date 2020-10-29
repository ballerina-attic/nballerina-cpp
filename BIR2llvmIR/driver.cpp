#include "BIR.h"
#include "BIRReader.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv)
{
  int opt;
  string inFileName = "";
  string outFileName = "";
  string exeName = "";
  if (argc <= 1 ) {
     printf("Need input file name \n");
     exit(0);
  }
  // put ':' in the starting of the 
  // string so that program can  
  //distinguish between '?' and ':'  
  while((opt = getopt(argc, argv, "co:")) != -1)
  {
    switch(opt)
    {
      case 'c':
	//exeName = optarg;
        break;
      case 'o':
	outFileName = optarg;
        break;
      case ':':
        break;
      case '?':
        break;
    }
  }
  // optind is for the extra arguments 
  // which are not parsed 
  for(; optind < argc; optind++){
    inFileName = argv[optind];
  }
  // if output file name is empty from command line options.
  if (outFileName == "") {
    for (unsigned int i = 0; i < inFileName.length(); i++) {
      char tmpstr = inFileName[i];
      if (tmpstr == '.')
        break;
      outFileName = outFileName + inFileName[i];
    }
  outFileName = outFileName + ".ll";
  }

  BIRReader *Reader = new BIRReader(inFileName);
  BIRPackage *bIRPackage = new BIRPackage ();
  Reader->deserialize(bIRPackage);
  char* Message;
  bool dumpllvm = true; //temp value
  string ModuleName = bIRPackage->getOrgName() + bIRPackage->getPackageName()
			 + bIRPackage->getVersion();
  LLVMModuleRef mod = LLVMModuleCreateWithName(ModuleName.c_str());
  LLVMSetSourceFileName(mod, bIRPackage->getSrcFileName().c_str(),
                        bIRPackage->getSrcFileName().length());
  LLVMSetDataLayout(mod, "e-m:e-i64:64-f80:128-n8:16:32:64-S128");
  LLVMSetTarget(mod, "x86_64-pc-linux-gnu");
  bIRPackage->translate(mod);
  
  if(dumpllvm)
  {
    if (LLVMPrintModuleToFile(mod, outFileName.c_str(), &Message))
    {
      std::cerr << Message;
    }  
  }
}
