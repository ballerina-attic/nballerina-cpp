#ifndef BIR_H
#define BIR_H

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "BalAbstractInsn.h"
#include "BalInvokableType.h"
#include "BalMoveInsn.h"
#include "BalNonTerminatorInsn.h"
#include "BalOperand.h"
#include "BalParam.h"
#include "BalTerminatorInsn.h"
#include "BalTypeDecl.h"
#include "BalVarDecl.h"
#include "Debuggable.h"
#include "PackageNode.h"
#include "Translatable.h"
#include "llvm-c/Core.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/MC/StringTableBuilder.h"
#include "llvm/Support/raw_ostream.h"

#ifndef unix
#define __attribute__(unused)
#endif

#define DEFAULT_VERSION 0
using namespace std;
using namespace llvm;
using namespace nballerina;

// Forward Declarations
class BIRBasicBlock;
class BIRFunction;
class BIRPackage;

enum SymbolKind { LOCAL_SYMBOL_KIND, GLOBAL_SYMBOL_KIND };

enum TypeTagEnum {
  TYPE_TAG_INT = 1,
  TYPE_TAG_BYTE = 2,
  TYPE_TAG_FLOAT = 3,
  TYPE_TAG_DECIMAL = 4,
  TYPE_TAG_STRING = 5,
  TYPE_TAG_BOOLEAN = 6,
  TYPE_TAG_JSON = 7,
  TYPE_TAG_XML = 8,
  TYPE_TAG_TABLE = 9,
  TYPE_TAG_NIL = 10,
  TYPE_TAG_ANYDATA = 11,
  TYPE_TAG_RECORD = 12,
  TYPE_TAG_TYPEDESC = 13,
  TYPE_TAG_STREAM = 14,
  TYPE_TAG_MAP = 15,
  TYPE_TAG_INVOKABLE = 16,
  TYPE_TAG_ANY = 17,
  TYPE_TAG_ENDPOINT = 18,
  TYPE_TAG_ARRAY = 19,
  TYPE_TAG_UNION = 20,
  TYPE_TAG_INTERSECTION = 21,
  TYPE_TAG_PACKAGE = 22,
  TYPE_TAG_NONE = 23,
  TYPE_TAG_VOID = 24,
  TYPE_TAG_XMLNS = 25,
  TYPE_TAG_ANNOTATION = 26,
  TYPE_TAG_SEMANTIC_ERROR = 27,
  TYPE_TAG_ERROR = 28,
  TYPE_TAG_ITERATOR = 29,
  TYPE_TAG_TUPLE = 30,
  TYPE_TAG_FUTURE = 31,
  TYPE_TAG_FINITE = 32,
  TYPE_TAG_OBJECT = 33,
  TYPE_TAG_SERVICE = 34,
  TYPE_TAG_BYTE_ARRAY = 35,
  TYPE_TAG_FUNCTION_POINTER = 36,
  TYPE_TAG_HANDLE = 37,
  TYPE_TAG_READONLY = 38,
  TYPE_TAG_SIGNED32_INT = 39,
  TYPE_TAG_SIGNED16_INT = 40,
  TYPE_TAG_SIGNED8_INT = 41,
  TYPE_TAG_UNSIGNED32_INT = 42,
  TYPE_TAG_UNSIGNED16_INT = 43,
  TYPE_TAG_UNSIGNED8_INT = 44,
  TYPE_TAG_CHAR_STRING = 45,
  TYPE_TAG_XML_ELEMENT = 46,
  TYPE_TAG_XML_PI = 47,
  TYPE_TAG_XML_COMMENT = 48,
  TYPE_TAG_XML_TEXT = 49,
  TYPE_TAG_NEVER = 50,
  TYPE_TAG_NULL_SET = 51,
  TYPE_TAG_PARAMETERIZED_TYPE = 52
};

class ConstantLoadInsn : public NonTerminatorInsn {
private:
  enum TypeTagEnum typeTag;
  union value {
    int intValue;
    double floatValue;
    bool boolValue;
    string *strValue;
    value() {}
    value(int x) : intValue(x) {}
    value(float x) : floatValue(x) {}
    value(bool x) : boolValue(x) {}
    value(string *x) : strValue(x) {}
  } val;

public:
  ConstantLoadInsn() = default;
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   int intval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   float floatval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   bool boolval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   string *strval);
  ~ConstantLoadInsn() = default;

  int getIntValue() { return val.intValue; }
  float getFloatValue() { return val.floatValue; }
  bool getBoolValue() { return val.boolValue; }
  string *getStringValue() { return val.strValue; }
  void setIntValue(int intVal, TypeTagEnum TypeTag) {
    val.intValue = intVal;
    typeTag = TypeTag;
  }
  void setFloatValue(float floatVal, TypeTagEnum TypeTag) {
    val.floatValue = floatVal;
    typeTag = TypeTag;
  }
  void setBoolValue(bool boolVal, TypeTagEnum TypeTag) {
    val.boolValue = boolVal;
    typeTag = TypeTag;
  }
  void setStringValue(string *str, TypeTagEnum TypeTag) {
    val.strValue = str;
    typeTag = TypeTag;
  }
  // With Nil Type setting only Type Tag because value will be zero with NIL
  // Type.
  void setTypeTagNil(TypeTagEnum TypeTag) { typeTag = TypeTag; }
  TypeTagEnum getTypeTag() { return typeTag; }
  void translate(LLVMModuleRef &modRef) final;
};

class BinaryOpInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp1;
  Operand *rhsOp2;

public:
  BinaryOpInsn() = default;
  BinaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp1,
               Operand *rOp2);
  ~BinaryOpInsn() = default;

  Operand *getRhsOp1() { return rhsOp1; }
  Operand *getRhsOp2() { return rhsOp2; }
  void setRhsOp1(Operand *op) { rhsOp1 = op; }
  void setRhsOp2(Operand *op) { rhsOp2 = op; }

  void translate(LLVMModuleRef &modRef) final;
};

class UnaryOpInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;

public:
  UnaryOpInsn() = default;
  UnaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp);
  ~UnaryOpInsn() = default;
  Operand *getRhsOp() { return rhsOp; }

  void setRhsOp(Operand *op) { rhsOp = op; }

  void translate(LLVMModuleRef &modRef) final;
};

class TypeCastInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  TypeDecl *typeDecl;
  bool checkTypes;

public:
  TypeCastInsn() = default;
  TypeCastInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp,
               TypeDecl *tDecl, bool checkTypes);
  ~TypeCastInsn() = default;

  Operand *getRhsOp() { return rhsOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  bool mustCheckTypes() { return checkTypes; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void setTypeDecl(TypeDecl *tDecl) { typeDecl = tDecl; }
  void setTypesChecking(bool checktypes) { checkTypes = checktypes; }
  void translate(LLVMModuleRef &modRef) final;
};

class TypeTestInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  TypeDecl *typeDecl;

public:
  TypeTestInsn() = default;
  TypeTestInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp,
               TypeDecl *tDecl);
  ~TypeTestInsn() = default;

  Operand *getRhsOp() { return rhsOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void setTypeDecl(TypeDecl *tDecl) { typeDecl = tDecl; }
};

class ArrayInsn : public NonTerminatorInsn {
private:
  Operand *sizeOp;
  TypeDecl *typeDecl;

public:
  ArrayInsn() = default;
  ArrayInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *sOp,
            TypeDecl *TDecl);
  void setSizeOp(Operand *Size) { sizeOp = Size; }
  void setTypeDecl(TypeDecl *Type) { typeDecl = Type; }

  Operand *getSizeOp() { return sizeOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  void translate(LLVMModuleRef &modRef) final;
  LLVMValueRef getNewArrayDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
  ~ArrayInsn() = default;
};

class ArrayLoadInsn : public NonTerminatorInsn {
private:
  bool optionalFieldAccess;
  bool fillingRead;
  Operand *keyOp;
  Operand *rhsOp;

public:
  ArrayLoadInsn() = default;
  ArrayLoadInsn(Location *pos, InstructionKind kind, Operand *lOp, bool opFA,
                bool fR, Operand *KOp, Operand *ROp);
  void setOptionalFieldAccess(bool OpFAccess) {
    optionalFieldAccess = OpFAccess;
  }
  void setFillingRead(bool fRead) { fillingRead = fRead; }
  void setKeyOp(Operand *kOp) { keyOp = kOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }
  bool getOptionalFieldAccess() { return optionalFieldAccess; }
  bool getFillingRead() { return fillingRead; }
  Operand *getKeyOp() { return keyOp; }
  Operand *getRhsOp() { return rhsOp; }
  ~ArrayLoadInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
  LLVMValueRef getArrayLoadDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
};

class ArrayStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;

public:
  ArrayStoreInsn() = default;
  ArrayStoreInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 Operand *KOp, Operand *ROp);
  void setKeyOp(Operand *kOp) { keyOp = kOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }
  Operand *getKeyOp() { return keyOp; }
  Operand *getRhsOp() { return rhsOp; }
  ~ArrayStoreInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
  LLVMValueRef getArrayStoreDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
};

class MapStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;
  LLVMValueRef getMapIntStoreDeclaration(LLVMModuleRef &modRef,
                                         BIRPackage *pkg);

public:
  MapStoreInsn() = default;
  MapStoreInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *KOp,
               Operand *ROp);
  void setKeyOp(Operand *kOp) { keyOp = kOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }
  Operand *getKeyOp() { return keyOp; }
  Operand *getRhsOp() { return rhsOp; }
  ~MapStoreInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
};

class ConditionBrInsn : public TerminatorInsn {
private:
  BIRBasicBlock *ifThenBB;
  BIRBasicBlock *elseBB;

public:
  ConditionBrInsn() = default;
  ConditionBrInsn(Location *pos, InstructionKind kind, Operand *lOp,
                  BIRBasicBlock *nextB);
  ~ConditionBrInsn() = default;
  void setIfThenBB(BIRBasicBlock *ifBB) { ifThenBB = ifBB; }
  void setElseBB(BIRBasicBlock *elseB) { elseBB = elseB; }

  BIRBasicBlock *getIfThenBB() { return ifThenBB; }
  BIRBasicBlock *getElseBB() { return elseBB; }

  void translate(LLVMModuleRef &modRef) final;
};

class GoToInsn : public TerminatorInsn {
private:
  LLVMValueRef llvmInsn;

public:
  GoToInsn() = default;
  GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
           BIRBasicBlock *nextBB);
  ~GoToInsn() = default;

  LLVMValueRef getLLVMInsn() { return llvmInsn; }
  void setLLVMInsn(LLVMValueRef insn) { llvmInsn = insn; }

  void translate(LLVMModuleRef &modRef) final;
};

class ReturnInsn : public TerminatorInsn {
private:
public:
  ReturnInsn() = default;
  ReturnInsn(Location *pos, InstructionKind kind, Operand *lOp,
             BIRBasicBlock *nextBB);
  ~ReturnInsn() = default;

  void translate(LLVMModuleRef &modRef) final;
};

class TypeDescInsn : public NonTerminatorInsn {
public:
  TypeDescInsn() = default;
  ~TypeDescInsn() = default;
};

class StructureInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  void mapInsnTranslate(VarDecl *lhsVar, LLVMModuleRef &modRef);
  LLVMValueRef getNewMapIntDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);

public:
  StructureInsn() = default;
  ~StructureInsn() = default;
  Operand *getRhsOp() { return rhsOp; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void translate(LLVMModuleRef &modRef) final;
};

class FunctionCallInsn : public TerminatorInsn {
private:
  bool isVirtual;
  string functionName;
  int argCount;
  vector<Operand *> argsList;
  Param *restParam;

public:
  FunctionCallInsn() = default;
  void setIsVirtual(bool funcVirtual) { isVirtual = funcVirtual; }
  void setFunctionName(string funcName) { functionName = funcName; }
  void setArgCount(int argNumber) { argCount = argNumber; }
  void setArgumentsList(vector<Operand *> fnArgs) { argsList = fnArgs; }
  void addArgumentToList(Operand *arg) { argsList.push_back(arg); }
  void setRestParam(Param *rParam) { restParam = rParam; }

  bool getIsVirtual() { return isVirtual; }
  string getFunctionName() { return functionName; }
  int getArgCount() { return argCount; }
  Operand *getArgumentFromList(int i) { return argsList[i]; }
  vector<Operand *> getArgumentsList() { return argsList; }
  FunctionCallInsn(string funcName, bool funcVirtual, int argNumber,
                   Operand *lhsOp, BIRBasicBlock *thenBB);
  Param *getRestParam() { return restParam; }
  ~FunctionCallInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
};

class BIRBasicBlock : public PackageNode,
                      public Debuggable,
                      public Translatable {
private:
  string id;
  vector<NonTerminatorInsn *> instructions;
  TerminatorInsn *terminator;
  LLVMBuilderRef bRef;
  BIRFunction *bFunc;
  BIRBasicBlock *nextBB;
  LLVMBasicBlockRef bbRefObj;

public:
  BIRBasicBlock() = default;
  BIRBasicBlock(string id);
  ~BIRBasicBlock() = default;
  BIRBasicBlock(Location *pos, string id);

  string getId() { return id; }
  TerminatorInsn *getTerminatorInsn() { return terminator; }
  LLVMBuilderRef getLLVMBuilderRef() { return bRef; }
  BIRFunction *getBIRFunction() { return bFunc; }
  BIRBasicBlock *getNextBB() { return nextBB; }
  vector<NonTerminatorInsn *> getNonTerminatorInsn() { return instructions; }
  NonTerminatorInsn *getInsn(int i) { return instructions[i]; }
  size_t numInsns() { return instructions.size(); }
  LLVMBasicBlockRef getLLVMBBRef() { return bbRefObj; }

  void setId(string newId) { id = newId; }
  void setTerminatorInsn(TerminatorInsn *insn) { terminator = insn; }
  void setLLVMBuilderRef(LLVMBuilderRef buildRef) { bRef = buildRef; }
  void setBIRFunction(BIRFunction *func) { bFunc = func; }
  void setNextBB(BIRBasicBlock *bb) { nextBB = bb; }
  void setLLVMBBRef(LLVMBasicBlockRef bbRef) { bbRefObj = bbRef; }
  void addNonTermInsn(NonTerminatorInsn *insn) { instructions.push_back(insn); }

  void translate(LLVMModuleRef &modRef) final;
};

class FuncParam : public VarDecl {
private:
  bool hasDefaultExpr;

public:
  FuncParam() = default;
  FuncParam(Location *pos, TypeDecl *ty, string name, string metaName,
            VarKind k, VarScope sc, int off, bool hasDefExp);
  ~FuncParam() = default;

  bool hasDefaultValue() { return hasDefaultExpr; }
};

class BIRFunction : public PackageNode, public Debuggable, public Translatable {
private:
  string name;
  int flags;
  InvokableType *type;
  vector<Operand *> requiredParams;
  VarDecl *receiver;
  Param *restParam;
  int paramCount;
  map<string, VarDecl *> localVars;
  VarDecl *returnVar;

  vector<BIRBasicBlock *> basicBlocks;
  string workerName;
  LLVMBuilderRef builder;
  LLVMValueRef newFunction;
  map<FuncParam *, vector<BIRBasicBlock *>> params;
  map<string, LLVMValueRef> localVarRefs;
  map<string, LLVMValueRef> branchComparisonList;

public:
  BIRFunction() = default;
  BIRFunction(Location *pos, string pname, int pflags, InvokableType *ptype,
              string pworkerName);
  BIRFunction(const BIRFunction &) = delete;
  ~BIRFunction() = default;

  string getName() { return name; }
  int getFlags() { return flags; }
  InvokableType *getInvokableType() { return type; }
  vector<Operand *> getParams() { return requiredParams; }
  Operand *getParam(int i) { return requiredParams[i]; }
  VarDecl *getReceiver() { return receiver; }
  Param *getRestParam() { return restParam; }

  VarDecl *getReturnVar() { return returnVar; }
  vector<BIRBasicBlock *> getBasicBlocks() { return basicBlocks; }
  size_t numBasicBlocks() { return basicBlocks.size(); }
  BIRBasicBlock *getBasicBlock(int i) { return basicBlocks[i]; }
  string getWorkerName() { return workerName; }
  LLVMBuilderRef getLLVMBuilder() { return builder; }
  int getNumParams() { return paramCount; }
  LLVMValueRef getNewFunctionRef() { return newFunction; }
  map<string, LLVMValueRef> getLocalVarRefs() { return localVarRefs; }
  map<string, LLVMValueRef> getBranchComparisonList() {
    return branchComparisonList;
  }
  LLVMValueRef getValueRefBasedOnName(string lhsName);

  void setName(string newName) { name = newName; }
  void setFlags(int newFlags) { flags = newFlags; }
  void setInvokableType(InvokableType *t) { type = t; }
  void setParams(vector<Operand *> p) { requiredParams = p; }
  void setParam(Operand *param) { requiredParams.push_back(param); }
  void setReceiver(VarDecl *var) { receiver = var; }
  void setRestParam(Param *param) { restParam = param; }
  void setNumParams(int paramcount) { paramCount = paramcount; }
  void setLocalVar(string name, VarDecl *var) {
    localVars.insert(pair<string, VarDecl *>(name, var));
  }
  void setReturnVar(VarDecl *var) { returnVar = var; }
  void setBIRBasicBlocks(vector<BIRBasicBlock *> b) { basicBlocks = b; }
  void addBIRBasicBlock(BIRBasicBlock *bb) { basicBlocks.push_back(bb); }
  void setWorkerName(string newName) { workerName = newName; }
  void setLLVMBuilder(LLVMBuilderRef b) { builder = b; }
  void setLocalVarRefs(map<string, LLVMValueRef> newLocalVarRefs) {
    localVarRefs = newLocalVarRefs;
  }
  void setNewFunctionRef(LLVMValueRef newFuncRef) { newFunction = newFuncRef; }

  void setBranchComparisonlist(map<string, LLVMValueRef> brCompl) {
    branchComparisonList = brCompl;
  }
  void addNewbranchComparison(string name, LLVMValueRef compRef) {
    branchComparisonList.insert(std::pair<string, LLVMValueRef>(name, compRef));
  }

  BIRBasicBlock *searchBb(std::string name);

  LLVMTypeRef getLLVMTypeRefOfType(TypeDecl *typeD);
  LLVMValueRef getLocalVarRefUsingId(string locVar);
  LLVMValueRef getLocalToTempVar(Operand *op);
  void translateFunctionBody(LLVMModuleRef &modRef);
  void patchInsn(Function *llvnFun);
  LLVMTypeRef getLLVMFuncRetTypeRefOfType(VarDecl *vDecl);
  VarDecl *getNameVarDecl(string opName);
  const char *getTypeNameOfTypeTag(TypeTagEnum typeTag);
  void translate(LLVMModuleRef &modRef) final;
};

class BIRPackage : public Translatable {
private:
  string org;
  string name;
  string version;
  string sourceFileName;
  vector<BIRFunction *> functions;
  vector<VarDecl *> globalVars;
  map<string, LLVMValueRef> globalVarRefs;
  map<string, BIRFunction *> functionLookUp;
  StructType *structType;
  StringTableBuilder *strBuilder;
  map<string, vector<LLVMValueRef>> structElementStoreInst;
  map<string, LLVMValueRef> arrayFunctionRefs;

public:
  BIRPackage() = default;
  BIRPackage(string orgName, string pkgName, string verName,
             string srcFileName);
  ~BIRPackage() = default;

  string getOrgName() { return org; }
  string getPackageName() { return name; }
  string getVersion() { return version; }
  string getSrcFileName() { return sourceFileName; }
  StringTableBuilder *getStrTableBuilder() { return strBuilder; }
  void setOrgName(string orgName) { org = orgName; }
  void setPackageName(string pkgName) { name = pkgName; }
  void setVersion(string verName) { version = verName; }
  void setSrcFileName(string srcFileName) { sourceFileName = srcFileName; }
  vector<BIRFunction *> getFunctions() { return functions; }
  BIRFunction *getFunction(int i) { return functions[i]; }
  vector<VarDecl *> getGlobalVars() { return globalVars; }
  map<string, LLVMValueRef> getGlobalVarRefs() { return globalVarRefs; }
  StructType *getStructType() { return structType; }
  void setFunctions(vector<BIRFunction *> f) { functions = f; }
  void addFunction(BIRFunction *f) { functions.push_back(f); }
  void addGlobalVar(VarDecl *g) { globalVars.push_back(g); }

  void addFunctionLookUpEntry(string funcName, BIRFunction *BIRfunction) {
    functionLookUp.insert(pair<string, BIRFunction *>(funcName, BIRfunction));
  }
  BIRFunction *getFunctionLookUp(string funcName) {
    return functionLookUp.at(funcName);
  }
  size_t numFunctions() { return functions.size(); }
  LLVMValueRef getGlobalVarRefUsingId(string globVar);
  void addStringOffsetRelocationEntry(string eleType, LLVMValueRef storeInsn);
  void applyStringOffsetRelocations(LLVMModuleRef &modRef);
  void addArrayFunctionRef(string arrayName, LLVMValueRef functionRef) {
    arrayFunctionRefs.insert(
        std::pair<string, LLVMValueRef>(arrayName, functionRef));
  }
  LLVMValueRef getFunctionRefBasedOnName(string arrayName);
  map<string, LLVMValueRef> getArrayFuncRefMap() { return arrayFunctionRefs; }
  void translate(LLVMModuleRef &modRef) final;
};

#endif // BIR_H
