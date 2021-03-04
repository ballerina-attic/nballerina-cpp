#include "BIR.h"
#include "BIRReader.h"
#include <fstream>
#include <iostream>
#include <string>

#include <vector>

using namespace std;

BIRReader BIRReader::reader;

int main(int argc, char **argv) {
  string inFileName = "";
  string outFileName = "";
  string exeName = "";
  if (argc <= 1) {
    printf("Need input file name \n");
    exit(0);
  }
  
  int i = 0; 
  while (i < argc) { 
     string arg = std::string(argv[i]); 
     if (arg == "-c") { 
       i = i + 1; 
     } else if (arg == "-o") { 
       outFileName = argv[i + 1]; 
       i = i + 2; 
     } else { 
       inFileName = argv[i]; 
       i = i + 1; 
     } 
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
  BIRPackage &birPackage = BIRReader::reader.birPackage;
  char *message;
  bool dumpLlvm = true; // temp value
  string moduleName = birPackage.getOrgName() + birPackage.getPackageName() +
                      birPackage.getVersion();
  LLVMModuleRef mod = LLVMModuleCreateWithName(moduleName.c_str());
  const char *tripleStr = LLVM_DEFAULT_TARGET_TRIPLE;

  //MacOS specific code. This is needed, since the default Triple will have the OS as darwin, 
  //but the clang will expect the os as macosx
  Triple triple(LLVM_DEFAULT_TARGET_TRIPLE);
  char modifiedTriple[200];
  if(triple.isMacOSX()){
    unsigned major, minor, micro;
    if (triple.getMacOSXVersion(major, minor, micro)){
      triple.setOS(Triple::OSType::MacOSX);
      sprintf(modifiedTriple, "%s%i.%i.%i", triple.getTriple().c_str(), major, minor, micro);
      tripleStr = modifiedTriple;
    }
  }
  
  LLVMSetSourceFileName(mod, birPackage.getSrcFileName().c_str(),
                        birPackage.getSrcFileName().length());
  LLVMSetDataLayout(mod, "e-m:e-i64:64-f80:128-n8:16:32:64-S128");
  LLVMSetTarget(mod, tripleStr);
  birPackage.translate(mod);

  if (dumpLlvm) {
    if (LLVMPrintModuleToFile(mod, outFileName.c_str(), &message)) {
      std::cerr << message;
    }
  }
}
