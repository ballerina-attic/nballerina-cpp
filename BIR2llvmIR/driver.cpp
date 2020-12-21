#include "BIR.h"
#include "BIRReader.h"
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include <unistd.h>

using namespace std;

BIRReader BIRReader::reader;

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

  BIRReader::reader.setFileStream(inFileName);
  BIRReader::reader.deserialize();
  char* message;
  bool dumpLlvm = true; //temp value
  string moduleName = BIRReader::reader.birPackage.getOrgName() + BIRReader::reader.birPackage.getPackageName()
			 + BIRReader::reader.birPackage.getVersion();
  LLVMModuleRef mod = LLVMModuleCreateWithName(moduleName.c_str());
  LLVMSetSourceFileName(mod, BIRReader::reader.birPackage.getSrcFileName().c_str(),
                        BIRReader::reader.birPackage.getSrcFileName().length());
  LLVMSetDataLayout(mod, "e-m:e-i64:64-f80:128-n8:16:32:64-S128");
  LLVMSetTarget(mod, "x86_64-pc-linux-gnu");
  BIRReader::reader.birPackage.translate(mod);
  
  if(dumpLlvm)
  {
    if (LLVMPrintModuleToFile(mod, outFileName.c_str(), &message))
    {
      std::cerr << message;
    }  
  }
}
