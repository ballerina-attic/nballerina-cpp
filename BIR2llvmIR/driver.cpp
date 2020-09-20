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
#if 0
  string inFileName = "";
  string outFileName = "";
  string exeFileName = "";
  if( argv[1] != "") {
    inFileName = argv[1];
  }
  for (int i=0; i < argc; i++) {
    string argcArg = argv[i];
    if (argcArg == "-o") {
      outFileName = argv[i+1];
    }
    if (argv[i] == "-c") {
      exeFileName = argv[i+1];
    }
  }
  if (outFileName == "") {
    for (unsigned int i = 0; i < inFileName.length(); i++) {
      char tmpstr = inFileName[i];
      if (tmpstr == '.')
	break;
      outFileName = outFileName + inFileName[i];
     }
     outFileName = outFileName + ".ll";
  }
#endif
  class BIRReader *Reader = new BIRReader(inFileName);
  class BIRPackage *BIRpackage = new BIRPackage ();
  BIRpackage = Reader->deserialize(BIRpackage);
  cout << "\nRead successfull\n";
  char* Message;
  bool dumpllvm = true; //temp valuea
  string ModuleName = BIRpackage->getOrgName() + BIRpackage->getPackageName()
			 + BIRpackage->getVersion();
  LLVMModuleRef mod = LLVMModuleCreateWithName(ModuleName.c_str());
  LLVMSetSourceFileName(mod, BIRpackage->getSrcFileName().c_str(),
                        BIRpackage->getSrcFileName().length());
  LLVMSetDataLayout(mod, "e-m:e-i64:64-f80:128-n8:16:32:64-S128");
  LLVMSetTarget(mod, "x86_64-pc-linux-gnu");
  BIRpackage->translate(mod);
  
  if(dumpllvm)
  {
    if (LLVMPrintModuleToFile(mod, outFileName.c_str(), &Message))
    {
      std::cerr << Message;
    }  
  }
#if 0
  Location *loc1 = new Location("newFile",2,9);
  VarDecl *vDecllhs = new VarDecl(loc1, "1stVarLhs", "1metanameLhs");
  TypeDecl *tyDecl11 = new TypeDecl();
  tyDecl11->setTypeDeclName ("int");
  vDecllhs->setTypeDecl(tyDecl11);

  Operand *lOp = new Operand(vDecllhs);
  VarDecl *vDeclrhs = new VarDecl(loc1, "1stVarRhs", "tmetanameRhs");
  TypeDecl *tyDecl2 = new TypeDecl();
  tyDecl2->setTypeDeclName ("int");
  vDeclrhs->setTypeDecl(tyDecl2);  

  Operand *rOp = new Operand(vDeclrhs);
  MoveInsn *moveInsn = new MoveInsn(loc1, INS_KIND_MOVE, lOp, rOp);
  NonTerminatorInsn *NonTInsn = static_cast<NonTerminatorInsn*>(moveInsn);
  BasicBlockT *bb = new BasicBlockT("BB1");
  bb->addNonTermInsn(NonTInsn);
  TypeDecl *tyDecl = new TypeDecl();
  tyDecl->setTypeDeclName ("void");
  InvokableType *invokT = new InvokableType();
  invokT->setReturnType(tyDecl);
  
  BIRFunction *func = new BIRFunction(loc1, "myFunc", 10, invokT, "MyWorker");
  func->addBasicBlockT(bb);
  func->setLocalVar(vDecllhs);
  func->setLocalVar(vDeclrhs);
  VarDecl *retVar = new VarDecl(loc1, "void", "metaName");
  TypeDecl *tyDecl1 = new TypeDecl();
  tyDecl1->setTypeDeclName ("void");
  retVar->setTypeDecl(tyDecl1);
  func->setReturnVar(retVar);
  BIRPackage *pkg = new BIRPackage ("$anon", ".", "0.0.0", "sourceFile");
  pkg->addFunction(func);
  pkg->translate(mod);

  if(dumpllvm)
  {
    string targetFineName = "foo.ll";
    LLVMPrintModuleToFile(mod, targetFineName.c_str(), &Message);
  }
#endif
}
