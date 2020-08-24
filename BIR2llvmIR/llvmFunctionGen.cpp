#include "BIR.h"

BIRFunction::BIRFunction()
{
}

BIRFunction::BIRFunction(Location pos, string name, int flags, InvokableType type,
                string workerName)
{
  
}

BIRFunction::BIRFunction(const BIRFunction&) 
{
}

void BIRFunction::translate (LLVMModuleRef modRef)
{
  cout <<"I am inside BIRFunction Translate Function\n";
  LLVMContext ctx;
  Module *M = new Module("MyModule", ctx);

  FunctionType *FT =
    FunctionType::get(Type::getInt32Ty(ctx), /*not vararg*/false);

  Function *F = Function::Create(FT, Function::ExternalLinkage, "main", M);

/*  BIRPackage pkg; // temp obj. how we will get pkg details?
  string ModuleName = pkg.org + pkg.name + pkg.version;
  LLVMModuleRef mod = LLVMModuleCreateWithName(moduleID); // how we can call LLVM function ?*/ 

//  LLVMTypeRef *retType = new LLVMTypeRef[returnVar]; // how we can use LLVMTypeRef types ?i how to generate LLVMTypeRe                                                     // using returne type.
  LLVMTypeRef funcType;
  unsigned int numParams = paramCount; 
  bool isVarArg = false;
  //LLVMTypeRef *paramTypes  = new LLVMTypeRef[numParams]; // 
  

/*  for (unsigned i = 0;  i < numParams;  i++)
    paramTypes[i] = // here, inside InvokableType class have paramType list. We have to get details from that list? 

  
  funcType = LLVMFunctionType(retType, paramTypes, numParams, isVarArg);

  const char *newFuncName = name;

  LLVMValueRef newFunction;

  newFunction = LLVMAddFunction(mod, newFuncName, funcType);
   
  LLVMDumpModule(mod);*/
}

BIRFunction::~BIRFunction()
{
}
