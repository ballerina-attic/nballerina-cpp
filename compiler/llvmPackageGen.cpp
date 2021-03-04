#include "BIR.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

BIRPackage::BIRPackage() {}

BIRPackage::BIRPackage(string orgName, string pkgName, string verName,
                       string srcFileName)
    : org(orgName), name(pkgName), version(verName),
      sourceFileName(srcFileName) {}

// return ValueRef of global variable based on variable name.
LLVMValueRef BIRPackage::getGlobalVarRefUsingId(string globVar) {
  map<string, LLVMValueRef>::iterator it;
  it = globalVarRefs.find(globVar);
  if (it == globalVarRefs.end()) {
    return NULL;
  } else {
    return it->second;
  }
}

void BIRPackage::translate(LLVMModuleRef &modRef) {
  // String Table initialization
  strBuilder = new StringTableBuilder(StringTableBuilder::RAW, 1);
  // iterate over all global variables and translate
  for (auto const it : globalVars) {
    LLVMValueRef globVarRef;
    VarDecl *globVar = it.second;
    BIRFunction *funcObj = new BIRFunction();
    LLVMTypeRef varTyperef =
        funcObj->getLLVMTypeRefOfType(globVar->getTypeDecl());
    string varName = globVar->getVarName();
    if (varTyperef && modRef) {
      // emit/adding the global variable.
      Constant *initValue = Constant::getNullValue(unwrap(varTyperef));
      GlobalVariable *gVar = new GlobalVariable(
          *unwrap(modRef), unwrap(varTyperef), false,
          GlobalValue::ExternalLinkage, initValue, varName.c_str(), 0);
      gVar->setAlignment(Align(4));
      globVarRef = wrap(gVar);
    }
    if (globVarRef)
      globalVarRefs.insert({globVar->getVarName(), globVarRef});
  }

  // creating struct smart pointer to store any type variables data.
  LLVMTypeRef structGen =
      LLVMStructCreateNamed(LLVMGetGlobalContext(), "struct.smtPtr");
  LLVMTypeRef *structElementTypes = new LLVMTypeRef[3];
  structElementTypes[0] = LLVMInt32Type();
  structElementTypes[1] = LLVMInt32Type();
  structElementTypes[2] = LLVMPointerType(LLVMInt8Type(), 0);
  LLVMStructSetBody(structGen, structElementTypes, 3, 0);
  structType = unwrap<StructType>(structGen);

  // iterating over each function, first create function definition
  // (without function body) and adding to Module.
  map<string, BIRFunction *>::iterator it;
  for (it = functionLookUp.begin(); it != functionLookUp.end(); it++) {
    BIRFunction *birFunc = it->second;
    LLVMBuilderRef builder = LLVMCreateBuilder();
    birFunc->setLLVMBuilder(builder);
    LLVMTypeRef funcType;
    LLVMTypeRef retType;
    unsigned int numParams = birFunc->getNumParams();
    LLVMTypeRef *paramTypes = new LLVMTypeRef[numParams];
    bool isVarArg = false;
    birFunc->setPkgAddress(this);
    if (birFunc->getRestParam())
      isVarArg = true;

    if (birFunc->getReturnVar())
      retType = birFunc->getLLVMFuncRetTypeRefOfType(birFunc->getReturnVar());
    for (unsigned i = 0; i < numParams; i++) {
      Operand *funcParam = birFunc->getParam(i);
      if (funcParam && funcParam->typeTag() == TYPE_TAG_ANY) {
        paramTypes[i] = wrap(structType);
      } else {
        paramTypes[i] = birFunc->getLLVMTypeRefOfType(
            funcParam->getVarDecl()->getTypeDecl());
      }
    }
    if (retType) {
      funcType = LLVMFunctionType(retType, paramTypes, numParams, isVarArg);
      if (funcType) {
        birFunc->setNewFunctionRef(
            LLVMAddFunction(modRef, birFunc->getName().c_str(), funcType));
      }
    }
  }

  // iterating over each function translate the function body.
  map<string, BIRFunction *>::iterator it1;
  for (it1 = functionLookUp.begin(); it1 != functionLookUp.end(); it1++) {
    BIRFunction *birFunc = it1->second;
    birFunc->setPkgAddress(this);
    birFunc->translate(modRef);
  }
  // This Api will finalize the string table builder if table size is not
  // zero.
  if (strBuilder->getSize() != 0)
    applyStringOffsetRelocations(modRef);
}

void BIRPackage::addStringOffsetRelocationEntry(string eleType,
                                                LLVMValueRef storeInsn) {
  if (structElementStoreInst.size() == 0) {
    vector<LLVMValueRef> temp;
    temp.push_back(storeInsn);
    structElementStoreInst.insert(
        pair<string, vector<LLVMValueRef>>(eleType, temp));
  } else {
    map<string, vector<LLVMValueRef>>::iterator itr;
    itr = structElementStoreInst.find(eleType);
    if (itr == structElementStoreInst.end()) {
      vector<LLVMValueRef> temp1;
      temp1.push_back(storeInsn);
      structElementStoreInst.insert(
          pair<string, vector<LLVMValueRef>>(eleType, temp1));
    } else {
      vector<LLVMValueRef> temp2 = itr->second;
      temp2.push_back(storeInsn);
      itr->second = temp2;
    }
  }
}

// Finalizing the string table after storing all the values into string table
// and Storing the any type data (string table offset).
void BIRPackage::applyStringOffsetRelocations(LLVMModuleRef &modRef) {
  strBuilder->finalize();
  map<string, vector<LLVMValueRef>>::iterator itr;
  for (itr = structElementStoreInst.begin();
       itr != structElementStoreInst.end(); itr++) {
    size_t finalOrigOffset = strBuilder->getOffset(itr->first);
    vector<LLVMValueRef> temp = itr->second;
    LLVMValueRef tempVal = LLVMConstInt(LLVMInt32Type(), finalOrigOffset, 0);
    for (unsigned int i = 0; i < temp.size(); i++) {
      LLVMValueRef storeInsn = temp[i];
      LLVMValueRef constOperand = LLVMGetOperand(storeInsn, 0);
      LLVMReplaceAllUsesWith(constOperand, tempVal);
      break;
    }
  }
}

LLVMValueRef BIRPackage::getFunctionRefBasedOnName(string arrayName) {
  map<string, LLVMValueRef>::iterator it;
  it = arrayFunctionRefs.find(arrayName);

  if (it == arrayFunctionRefs.end()) {
    return NULL;
  } else
    return it->second;
}

VarDecl *BIRPackage::getGlobalVarDeclFromName(string name) {
  auto varIt = globalVars.find(name);
  if (varIt == globalVars.end())
    return nullptr;

  return varIt->second;
}

BIRPackage::~BIRPackage() {}
