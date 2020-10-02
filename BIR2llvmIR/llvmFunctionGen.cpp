#include "BIR.h"

BIRFunction::BIRFunction()
{
}

BIRFunction::BIRFunction(Location *pos, string namep, int flagsp, 
			  InvokableType *typep, string workerNamep):
	BIRNode(pos), name(namep), flags(flagsp), type(typep), workerName(workerNamep)
{
  
}

BIRFunction::BIRFunction(const BIRFunction&) 
{
}

LLVMValueRef BIRFunction::getLocalVarRefUsingId(string locVar)
{
  for(std::map<string, LLVMValueRef>::iterator iter = localVarRefs.begin();
	 iter != localVarRefs.end(); iter++)
  {
    if (iter->first == locVar)
      return iter->second;
  }
  return NULL;
}

LLVMValueRef BIRFunction::getLocalToTempVar(Operand *operand)
{
  string refOp = operand->getVarDecl()->getVarName();
  string tempName = refOp + "_temp";
  LLVMValueRef locVRef = getLocalVarRefUsingId(refOp);
  return LLVMBuildLoad(builder, locVRef, tempName.c_str());
}

static bool isParamter (VarDecl *locVar)
{
  switch(locVar->getVarKind()) {
    case LOCAL_VAR_KIND:
    case TEMP_VAR_KIND:
    case RETURN_VAR_KIND:
    case GLOBAL_VAR_KIND:
    case SELF_VAR_KIND:
    case CONSTANT_VAR_KIND:
      return false;
    case ARG_VAR_KIND:
      return true;
    default:
      return false;
  }
}

LLVMTypeRef BIRFunction::getLLVMFuncRetTypeRefOfType(VarDecl *vDecl)
{
  int typeTag = 0;
  if (vDecl->getTypeDecl())
    typeTag = vDecl->getTypeDecl()->getTypeTag();
  // if main function return type is void, but user wants to return some 
  // value using _bal_result (global variable from BIR), change main function 
  // return type from void to global variable (_bal_result) type. 
  if (typeTag == TYPE_TAG_ENUM_TYPE_TAG_NIL ||
      typeTag == TYPE_TAG_ENUM_TYPE_TAG_VOID) {
    vector<VarDecl *>  globVars = getpkgAddress()->getGlobalVars();
    for (unsigned int i = 0; i < globVars.size(); i++) {
      VarDecl* globVar = globVars[i];
      if (globVar->getVarName() == "_bal_result") {
	typeTag = globVar->getTypeDecl()->getTypeTag();
	break;
      }
    }
  }
  
  switch (typeTag) {
    case TYPE_TAG_ENUM_TYPE_TAG_INT:
      return LLVMInt32Type();
    case TYPE_TAG_ENUM_TYPE_TAG_BYTE:
    case TYPE_TAG_ENUM_TYPE_TAG_FLOAT:
    case TYPE_TAG_ENUM_TYPE_TAG_BOOLEAN:
      return LLVMInt8Type();
    default:
      return LLVMVoidType();
  }
}

void BIRFunction::translateFunctionBody(LLVMModuleRef &modRef)
{
  LLVMBasicBlockRef BbRef;
  int paramIndex = 0;
  BbRef = LLVMAppendBasicBlock(newFunction, "entry");
  LLVMPositionBuilderAtEnd(builder, BbRef);

  // iterate through all local vars.
  for (unsigned int i=0; i < localVars.size(); i++)
  {
    VarDecl *locVar = localVars[i];
    const char *varName = (locVar->getVarName()).c_str();
    LLVMTypeRef varType = getLLVMTypeRefOfType(locVar->getTypeDecl());
    LLVMValueRef localVarRef = LLVMBuildAlloca(builder, varType, varName);
    localVarRefs.insert({locVar->getVarName(), localVarRef});
   
    if (isParamter(locVar)){
      LLVMValueRef parmRef = LLVMGetParam(newFunction, paramIndex);
      if (parmRef)
        LLVMBuildStore(builder, parmRef, localVarRef);
      paramIndex++;
    }   
  }
  
  // iterate through with each basic block in the function and create them
  // first.
  for (unsigned int i=0; i < basicBlocks.size(); i++)
  {
    BasicBlockT *bb = basicBlocks[i];
    char label[20];                                     
    sprintf(label, "<label>:%d", i);
    LLVMBasicBlockRef bbRef = LLVMAppendBasicBlock(this->getNewFunctionRef(),
                                                   bb->getId().c_str());
    bb->setLLVMBBRef(bbRef);
    bb->setBIRFunction(this);
    bb->setLLVMBuilderRef(builder);
    bb->setpkgAddress(getpkgAddress());
  }

  // creating branch to next basic block.
  if (basicBlocks[0] && basicBlocks[0]->getLLVMBBRef())
    LLVMBuildBr(builder, basicBlocks[0]->getLLVMBBRef());

  // Now translate the basic blocks (essentially add the instructions in them)
  for (unsigned int i=0; i < basicBlocks.size(); i++)
  {
    BasicBlockT *bb = basicBlocks[i];
    LLVMPositionBuilderAtEnd(builder, bb->getLLVMBBRef());
    bb->translate(modRef);
  }
}

void BIRFunction::translate (LLVMModuleRef &modRef)
{
  builder = LLVMCreateBuilder();
  LLVMTypeRef   funcType;
  LLVMTypeRef   retType;
  LLVMTypeRef  *paramTypes;
  unsigned int  numParams = getNumParams();
  bool          isVarArg = false;
  if (getRestParam())
    isVarArg = true;

  if (returnVar)  
    retType = getLLVMFuncRetTypeRefOfType(returnVar);
  paramTypes = new LLVMTypeRef[numParams];;
  for (unsigned i = 0;  i < numParams;  i++)
  {
    Param *funcParam = requiredParams[i];
    if (funcParam)
      paramTypes[i] = getLLVMTypeRefOfType(funcParam->getTypeDecl()); 
  }
  if (retType)
    funcType = LLVMFunctionType(retType, paramTypes, numParams, isVarArg);
  const char *newFuncName = name.c_str();
  if (funcType) 
    newFunction = LLVMAddFunction(modRef, newFuncName, funcType);
  translateFunctionBody(modRef);
}

BIRFunction::~BIRFunction()
{
}

LLVMTypeRef BIRFunction::getLLVMTypeRefOfType(TypeDecl *typeD)
{
  int typeTag = typeD->getTypeTag();
  switch (typeTag) {
    case TYPE_TAG_ENUM_TYPE_TAG_INT:
      return LLVMInt32Type();
    case TYPE_TAG_ENUM_TYPE_TAG_BYTE:
    case TYPE_TAG_ENUM_TYPE_TAG_FLOAT:
    case TYPE_TAG_ENUM_TYPE_TAG_BOOLEAN:
      return LLVMInt8Type();
    default: 
      return LLVMInt32Type();
  }
}

