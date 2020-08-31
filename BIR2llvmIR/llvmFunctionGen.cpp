#include "BIR.h"

//LLVMBuilderRef builder;
LLVMValueRef varAllocBB;
map<const char*, LLVMValueRef> localVarRefs;
LLVMValueRef newFunction;

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

vector<VarDecl> BIRFunction::getLocalVars()
{
  return localVars;
}

LLVMValueRef BIRFunction::getnewFunctionRef()
{
  newFunction;
}

LLVMValueRef BIRFunction::getlocalVarRefusingId(string locVar)
{
  const char* locvarChar = locVar.c_str();
  for(std::map<const char*, LLVMValueRef>::iterator iter = localVarRefs.begin();
	 iter != localVarRefs.end(); ++iter)
  {
    if (iter->first == locvarChar)
      return iter->second;
  }
}

LLVMValueRef BIRFunction::getLocalToTempVar(Operand operand)
{
  string refOp = operand.getvarDecl()->getvarName();
  string tempName = refOp + "_temp";
  LLVMValueRef locVRef = getlocalVarRefusingId(refOp);
  return LLVMBuildLoad(builder, locVRef, tempName.c_str());
}

static bool isParamter (VarDecl locVar)
{
  switch(locVar.getvarKind()) {
    case LocalVarKind:
    case TempVarKind:
    case ReturnVarKind:
      return false;
    case ArgVarKind:
      return true;
    default:
      return false;
  }
}

LLVMTypeRef BIRFunction::getllvmTypeRefOfType(TypeDecl typeD)
{
  string typeName = typeD.getTypeDeclName();
  if (typeName == "bool")
    return LLVMInt1Type();
  else if (typeName == "int")
    return LLVMInt32Type();
  else if (typeName == "float")
    return LLVMFloatType();
  else
    return LLVMVoidType();
}

void BIRFunction::translateFunctionBody(LLVMModuleRef modRef)
{
  LLVMBasicBlockRef BbRef;
  int paramIndex = 0;
  BbRef = LLVMAppendBasicBlock(newFunction, "var_allloc");
  LLVMPositionBuilderAtEnd(builder, BbRef);

  for (int i=0; i < localVars.size(); i++)
  {
    VarDecl locVar = localVars[i];
    const char *varName = (locVar.getvarName()).c_str();
    LLVMTypeRef varType = getllvmTypeRefOfType(locVar.gettypeDecl());
    LLVMValueRef localVarRef = LLVMBuildAlloca(builder, varType, varName);
    localVarRefs.insert({varName, localVarRef});
   
    if (isParamter(locVar)){
      LLVMValueRef parmRef = LLVMGetParam(newFunction, paramIndex);
      LLVMValueRef loaded = LLVMBuildStore(builder, parmRef, localVarRef);
      paramIndex++;
    }   
  }
  
  for (int i=0; i < basicBlocks.size(); i++)
  {
    BasicBlock1 *bb = basicBlocks[i];
    BasicBlock1 *bbnew = new BasicBlock1(builder, this, bb);
    bbnew->translate(modRef);
  }
}

void BIRFunction::translate (LLVMModuleRef modRef)
{
  cout <<"I am inside BIRFunction Translate Function\n";
  LLVMContext ctx;
  Module *M = new Module("MyModule", ctx);
  FunctionType *FT =
    FunctionType::get(Type::getInt32Ty(ctx), false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, "main", M);

  LLVMTypeRef   funcType;
  LLVMTypeRef   retType;
  LLVMTypeRef  *paramTypes;
  unsigned int  numParams = paramCount;
  bool          isVarArg = false;
  
  retType = getllvmTypeRefOfType(returnVar.gettypeDecl());
  paramTypes = new LLVMTypeRef[numParams];;
 
  for (unsigned i = 0;  i < numParams;  i++)
  {
    Param funcParam = requiredParams[i];
    paramTypes[i] = getllvmTypeRefOfType(funcParam.gettypeDecl()); 
  }
  
  funcType = LLVMFunctionType(retType, paramTypes, numParams, isVarArg);

  const char *newFuncName = name.c_str();

  newFunction = LLVMAddFunction(modRef, newFuncName, funcType);

  translateFunctionBody(modRef); 
   
}

LLVMBuilderRef BIRFunction::getllvmBuilder()
{
  return builder;
}
BIRFunction::~BIRFunction()
{
}
