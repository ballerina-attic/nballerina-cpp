#ifndef BIR_H
#define BIR_H

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

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

#define DEFAULT_VERSION 0
using namespace std;
using namespace llvm;

// Forward Declarations
class TypeDecl;
class BIRBasicBlock;
class VarDecl;
class BIRFunction;
class NonTerminatorInsn;
class BIRPackage;
class Param;

enum SymbolKind { LOCAL_SYMBOL_KIND, GLOBAL_SYMBOL_KIND };

enum VarKind {
  LOCAL_VAR_KIND = 1,
  ARG_VAR_KIND = 2,
  TEMP_VAR_KIND = 3,
  RETURN_VAR_KIND = 4,
  GLOBAL_VAR_KIND = 5,
  SELF_VAR_KIND = 6,
  CONSTANT_VAR_KIND = 7
};

enum InstructionKind {
  INSTRUCTION_KIND_GOTO = 1,
  INSTRUCTION_KIND_CALL = 2,
  INSTRUCTION_KIND_CONDITIONAL_BRANCH = 3,
  INSTRUCTION_KIND_RETURN = 4,
  INSTRUCTION_KIND_BINARY_REF_EQUAL,
  INSTRUCTION_KIND_BINARY_REF_NOT_EQUAL,
  INSTRUCTION_KIND_BINARY_CLOSED_RANGE,
  INSTRUCTION_KIND_BINARY_HALF_OPEN_RANGE,
  INSTRUCTION_KIND_BINARY_ANNOT_ACCESS,
  INSTRUCTION_KIND_BINARY_BITWISE_AND,
  INSTRUCTION_KIND_BINARY_BITWISE_OR,
  INSTRUCTION_KIND_BINARY_BITWISE_LEFT_SHIFT,
  INSTRUCTION_KIND_MOVE = 20,
  INSTRUCTION_KIND_CONST_LOAD = 21,
  INSTRUCTION_KIND_NEW_STRUCTURE,
  INSTRUCTION_KIND_NEW_MAP,
  INSTRUCTION_KIND_NEW_ARRAY = 25,
  INSTRUCTION_KIND_ARRAY_STORE = 26,
  INSTRUCTION_KIND_ARRAY_LOAD = 27,
  INSTRUCTION_KIND_TYPE_CAST = 29,
  INSTRUCTION_KIND_TYPE_TEST = 31,
  INSTRUCTION_KIND_NEW_TYPEDESC = 52,
  INSTRUCTION_KIND_BINARY_ADD = 61,
  INSTRUCTION_KIND_BINARY_SUB,
  INSTRUCTION_KIND_BINARY_MUL,
  INSTRUCTION_KIND_BINARY_DIV,
  INSTRUCTION_KIND_BINARY_MOD,
  INSTRUCTION_KIND_BINARY_EQUAL = 66,
  INSTRUCTION_KIND_BINARY_NOT_EQUAL = 67,
  INSTRUCTION_KIND_BINARY_GREATER_THAN = 68,
  INSTRUCTION_KIND_BINARY_GREATER_EQUAL = 69,
  INSTRUCTION_KIND_BINARY_LESS_THAN = 70,
  INSTRUCTION_KIND_BINARY_LESS_EQUAL = 71,
  INSTRUCTION_KIND_UNARY_NOT = 81,
  INSTRUCTION_KIND_UNARY_NEG,
  INSTRUCTION_KIND_BINARY_BITWISE_XOR = 85,
  INSTRUCTION_KIND_BINARY_BITWISE_UNSIGNED_RIGHT_SHIFT
}; // Enums are referred from bir-model.bal file

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

enum VarScope { VAR_SCOPE_GLOBAL = 1, VAR_SCOPE_FUNCTION = 2 };

class Location {
private:
  string fileName;
  int sLine;
  int sCol;
  int eLine;
  int eCol;

public:
  Location();
  Location(string name, int sline, int scol, int eline, int ecol);
  ~Location();
  void setFileName(string fname) { fileName = fname; }
  void setStartLineNum(int sline) { sLine = sline; }
  void setStartColumnNum(int scol) { sCol = scol; }
  void setEndLineNum(int eline) { eLine = eline; }
  void setEndColumnNum(int ecol) { eCol = ecol; }

  string getFileName() { return fileName; }
  int getStartLineNum() { return sLine; }
  int getStartColumnNum() { return sCol; }
  int getEndLineNum() { return eLine; }
  int getEndColumnNum() { return eCol; }

  void translate(LLVMModuleRef &modRef);
};

class BIRNode {
private:
  Location *loc;
  BIRPackage *pkgAddress;

public:
  BIRNode();
  BIRNode(Location *pos);
  virtual ~BIRNode();

  BIRPackage *getPkgAddress() { return pkgAddress; }
  Location *getLocation() { return loc; }
  void setLocation(Location *newLoc) { loc = newLoc; }
  void setPkgAddress(BIRPackage *pkgAddr) { pkgAddress = pkgAddr; }
  virtual void translate(LLVMModuleRef &modRef);
};

class TypeDecl {
private:
  int tag;
  string name;
  int flags;

public:
  TypeDecl();
  TypeDecl(int tag, int flags);
  TypeDecl(int tag, string name, int flags);
  ~TypeDecl();

  void setTypeDeclName(string namep) { name = namep; }
  void setTypeTag(int tagp) { tag = tagp; }
  void setFlags(int flag) { flags = flag; }

  int getTypeTag() { return tag; }
  string getTypeDeclName() { return name; }
  int getFlags() { return flags; }

  virtual void translate(LLVMModuleRef &modRef);
};

class Operand : public BIRNode {
private:
  VarDecl *varDecl;

public:
  Operand();
  Operand(VarDecl *vDecl);
  ~Operand();

  VarDecl *getVarDecl() { return varDecl; }
  void setVarDecl(VarDecl *newDecl) { varDecl = newDecl; }
  string name();
  int typeTag();
  void translate(LLVMModuleRef &modRef);
};

class AbstractInsn : public BIRNode {
private:
  InstructionKind kind;
  Operand *lhsOp;
  BIRFunction *bFunc;
  BIRBasicBlock *currentBB;

public:
  AbstractInsn();
  AbstractInsn(Location *pos, InstructionKind kind, Operand *lOp);
  ~AbstractInsn();

  InstructionKind getInstKind() { return kind; }
  Operand *getLhsOperand() { return lhsOp; }
  BIRFunction *getFunction() { return bFunc; }
  BIRBasicBlock *getCurrentBB() { return currentBB; }

  void setFunction(BIRFunction *func) { bFunc = func; }
  void setInstKind(InstructionKind newKind) { kind = newKind; }
  void setLhsOperand(Operand *lOp) { lhsOp = lOp; }
  void setCurrentBB(BIRBasicBlock *currB) { currentBB = currB; }
  void translate(LLVMModuleRef &modRef);
};

class NonTerminatorInsn : public AbstractInsn {
private:
public:
  NonTerminatorInsn();
  NonTerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp);
  ~NonTerminatorInsn();

  void translate(LLVMModuleRef &modRef);
};

class TerminatorInsn : public AbstractInsn {
private:
  BIRBasicBlock *thenBB;
  bool patchRequire;

public:
  TerminatorInsn();
  TerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 BIRBasicBlock *then);
  ~TerminatorInsn();

  BIRBasicBlock *getNextBB() { return thenBB; }
  bool getPatchStatus() { return patchRequire; }

  void setNextBB(BIRBasicBlock *block) { thenBB = block; }
  void setPatchStatus(bool patchrequire) { patchRequire = patchrequire; }

  void translate(LLVMModuleRef &modRef);
};

class MoveInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;

public:
  MoveInsn();
  MoveInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp);
  ~MoveInsn();

  Operand *getRhsOp() { return rhsOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }

  void translate(LLVMModuleRef &modRef);
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
  ConstantLoadInsn();
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   int intval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   float floatval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   bool boolval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   string *strval);
  ~ConstantLoadInsn();

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
  void translate(LLVMModuleRef &modRef);
};

class BinaryOpInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp1;
  Operand *rhsOp2;

public:
  BinaryOpInsn();
  BinaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp1,
               Operand *rOp2);
  ~BinaryOpInsn();

  Operand *getRhsOp1() { return rhsOp1; }
  Operand *getRhsOp2() { return rhsOp2; }
  void setRhsOp1(Operand *op) { rhsOp1 = op; }
  void setRhsOp2(Operand *op) { rhsOp2 = op; }

  void translate(LLVMModuleRef &modRef);
};

class UnaryOpInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;

public:
  UnaryOpInsn();
  UnaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp);
  ~UnaryOpInsn();
  Operand *getRhsOp() { return rhsOp; }

  void setRhsOp(Operand *op) { rhsOp = op; }

  void translate(LLVMModuleRef &modRef);
};

class TypeCastInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  TypeDecl *typeDecl;
  bool checkTypes;

public:
  TypeCastInsn();
  TypeCastInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp,
               TypeDecl *tDecl, bool checkTypes);
  ~TypeCastInsn();

  Operand *getRhsOp() { return rhsOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  bool mustCheckTypes() { return checkTypes; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void setTypeDecl(TypeDecl *tDecl) { typeDecl = tDecl; }
  void setTypesChecking(bool checktypes) { checkTypes = checktypes; }
  void translate(LLVMModuleRef &modRef);
};

class TypeTestInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;
  TypeDecl *typeDecl;

public:
  TypeTestInsn();
  TypeTestInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp,
               TypeDecl *tDecl);
  ~TypeTestInsn();

  Operand *getRhsOp() { return rhsOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void setTypeDecl(TypeDecl *tDecl) { typeDecl = tDecl; }

  void translate(LLVMModuleRef &modRef);
};

class ArrayInsn : public NonTerminatorInsn {
private:
  Operand *sizeOp;
  TypeDecl *typeDecl;

public:
  ArrayInsn();
  ArrayInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *sOp,
            TypeDecl *TDecl);
  void setSizeOp(Operand *Size) { sizeOp = Size; }
  void setTypeDecl(TypeDecl *Type) { typeDecl = Type; }

  Operand *getSizeOp() { return sizeOp; }
  TypeDecl *getTypeDecl() { return typeDecl; }
  void translate(LLVMModuleRef &modRef);
  LLVMValueRef getNewArrayDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
  ~ArrayInsn();
};

class ArrayLoadInsn : public NonTerminatorInsn {
private:
  bool optionalFieldAccess;
  bool fillingRead;
  Operand *keyOp;
  Operand *rhsOp;

public:
  ArrayLoadInsn();
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
  ~ArrayLoadInsn();
  void translate(LLVMModuleRef &modRef);
  LLVMValueRef getArrayLoadDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
};

class ArrayStoreInsn : public NonTerminatorInsn {
private:
  Operand *keyOp;
  Operand *rhsOp;

public:
  ArrayStoreInsn();
  ArrayStoreInsn(Location *pos, InstructionKind kind, Operand *lOp,
                 Operand *KOp, Operand *ROp);
  void setKeyOp(Operand *kOp) { keyOp = kOp; }
  void setRhsOp(Operand *rOp) { rhsOp = rOp; }
  Operand *getKeyOp() { return keyOp; }
  Operand *getRhsOp() { return rhsOp; }
  ~ArrayStoreInsn();
  void translate(LLVMModuleRef &modRef);
  LLVMValueRef getArrayStoreDeclaration(LLVMModuleRef &modRef, BIRPackage *pkg);
};

class ConditionBrInsn : public TerminatorInsn {
private:
  BIRBasicBlock *ifThenBB;
  BIRBasicBlock *elseBB;

public:
  ConditionBrInsn();
  ConditionBrInsn(Location *pos, InstructionKind kind, Operand *lOp,
                  BIRBasicBlock *nextB);
  ~ConditionBrInsn();
  void setIfThenBB(BIRBasicBlock *ifBB) { ifThenBB = ifBB; }
  void setElseBB(BIRBasicBlock *elseB) { elseBB = elseB; }

  BIRBasicBlock *getIfThenBB() { return ifThenBB; }
  BIRBasicBlock *getElseBB() { return elseBB; }

  void translate(LLVMModuleRef &modRef);
};

class GoToInsn : public TerminatorInsn {
private:
  LLVMValueRef llvmInsn;

public:
  GoToInsn();
  GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
           BIRBasicBlock *nextBB);
  ~GoToInsn();

  LLVMValueRef getLLVMInsn() { return llvmInsn; }
  void setLLVMInsn(LLVMValueRef insn) { llvmInsn = insn; }

  void translate(LLVMModuleRef &modRef);
};

class ReturnInsn : public TerminatorInsn {
private:
public:
  ReturnInsn();
  ReturnInsn(Location *pos, InstructionKind kind, Operand *lOp,
             BIRBasicBlock *nextBB);
  ~ReturnInsn();

  void translate(LLVMModuleRef &modRef);
};

class TypeDescInsn : public NonTerminatorInsn {
public:
  TypeDescInsn() {}
  ~TypeDescInsn() {}
};

class StructureInsn : public NonTerminatorInsn {
private:
  Operand *rhsOp;

public:
  StructureInsn() {}
  ~StructureInsn() {}
  Operand *getRhsOp() { return rhsOp; }
  void setRhsOp(Operand *op) { rhsOp = op; }
  void translate(LLVMModuleRef &modRef);
  LLVMValueRef getNewStructureDeclaration(LLVMModuleRef &modRef,
                                          BIRPackage *pkg);
};

class FunctionCallInsn : public TerminatorInsn {
private:
  bool isVirtual;
  string functionName;
  int argCount;
  vector<Operand *> argsList;
  Param *restParam;

public:
  FunctionCallInsn();
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
  ~FunctionCallInsn();
  void translate(LLVMModuleRef &modRef);
};

class BIRBasicBlock : public BIRNode {
private:
  string id;
  vector<NonTerminatorInsn *> instructions;
  TerminatorInsn *terminator;
  LLVMBuilderRef bRef;
  BIRFunction *bFunc;
  BIRBasicBlock *nextBB;
  LLVMBasicBlockRef bbRefObj;

public:
  BIRBasicBlock();
  BIRBasicBlock(string id);
  ~BIRBasicBlock();
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

  void translate(LLVMModuleRef &modRef);
};

class VarDecl : public BIRNode {
private:
  TypeDecl *type;
  string varName;
  string metaVarName;
  VarKind kind;
  VarScope scope;
  bool ignoreVariable;
  BIRBasicBlock *endBB;
  BIRBasicBlock *startBB;
  int insOffset;

public:
  VarDecl();
  VarDecl(Location *pos, string name, string metaName);
  VarDecl(Location *pos, TypeDecl *ty, string name, string metaName, VarKind k,
          VarScope sc, int offset);
  ~VarDecl();

  TypeDecl *getTypeDecl() { return type; }
  VarKind getVarKind() { return kind; }
  VarScope getVarScope() { return scope; }
  BIRBasicBlock *getStartBB() { return startBB; }
  BIRBasicBlock *getEndBB() { return endBB; }
  string getVarName() { return varName; }
  string getMetaVarName() { return metaVarName; }
  int getInsOffset() { return insOffset; }
  bool ignore() { return ignoreVariable; }

  void setTypeDecl(TypeDecl *newType) { type = newType; }
  void setVarKind(VarKind newKind) { kind = newKind; }
  void setVarScope(VarScope newScope) { scope = newScope; }
  void setStartBB(BIRBasicBlock *bb) { startBB = bb; }
  void setEndBB(BIRBasicBlock *bb) { endBB = bb; }
  void setVarName(string newName) { varName = newName; }
  void setMetaVarName(string newName) { metaVarName = newName; }
  void setInsOffset(int offset) { insOffset = offset; }
  void setIgnore(bool truth) { ignoreVariable = truth; }

  void translate(LLVMModuleRef &modRef);
};

class Param : public BIRNode {
private:
  string name;
  int flags;
  TypeDecl *type;

public:
  Param();
  Param(Location *pos, string name, int flags, TypeDecl *ty);
  ~Param();

  int getFlags() { return flags; }
  string getName() { return name; }
  TypeDecl *getTypeDecl() { return type; }

  void setFlags(int newFlags) { flags = newFlags; }
  void setName(string newName) { name = newName; }
  void setTypeDecl(TypeDecl *newType) { type = newType; }

  void translate(LLVMModuleRef &modRef);
};

class FuncParam : public VarDecl {
private:
  bool hasDefaultExpr;

public:
  FuncParam();
  FuncParam(Location *pos, TypeDecl *ty, string name, string metaName,
            VarKind k, VarScope sc, int off, bool hasDefExp);
  ~FuncParam();

  bool hasDefaultValue() { return hasDefaultExpr; }

  void translate(LLVMModuleRef &modRef);
};

class InvokableType : public TypeDecl {
private:
  vector<TypeDecl *> paramTypes;
  TypeDecl *restType;
  TypeDecl *returnType;

public:
  InvokableType();
  InvokableType(vector<TypeDecl *> paramTy, TypeDecl *restTy, TypeDecl *retTy);
  InvokableType(vector<TypeDecl *> paramTy, TypeDecl *retTy);
  ~InvokableType();

  TypeDecl *getReturnType() { return returnType; }
  TypeDecl *getRestType() { return restType; }
  TypeDecl *getParamType(int i) { return paramTypes[i]; }
  size_t getParamTypeCount() { return paramTypes.size(); }
  void setReturnType(TypeDecl *ty) { returnType = ty; }
  void setRestType(TypeDecl *ty) { restType = ty; }
  void addParamType(TypeDecl *ty) { paramTypes.push_back(ty); }

  void translate(LLVMModuleRef &modRef);
};

class BIRFunction : public BIRNode {
private:
  string name;
  int flags;
  InvokableType *type;
  vector<Operand *> requiredParams;
  VarDecl *receiver;
  Param *restParam;
  int paramCount;
  vector<VarDecl *> localVars;
  VarDecl *returnVar;

  vector<BIRBasicBlock *> basicBlocks;
  string workerName;
  LLVMBuilderRef builder;
  LLVMValueRef newFunction;
  map<FuncParam *, vector<BIRBasicBlock *>> params;
  map<string, LLVMValueRef> localVarRefs;
  map<string, LLVMValueRef> branchComparisonList;

public:
  BIRFunction();
  BIRFunction(Location *pos, string pname, int pflags, InvokableType *ptype,
              string pworkerName);
  BIRFunction(const BIRFunction &);
  ~BIRFunction();

  string getName() { return name; }
  int getFlags() { return flags; }
  InvokableType *getInvokableType() { return type; }
  vector<Operand *> getParams() { return requiredParams; }
  Operand *getParam(int i) { return requiredParams[i]; }
  VarDecl *getReceiver() { return receiver; }
  Param *getRestParam() { return restParam; }
  vector<VarDecl *> getLocalVars() { return localVars; }
  VarDecl *getLocalVar(int i) { return localVars[i]; }
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
  void setLocalVars(vector<VarDecl *> l) { localVars = l; }
  void setNumParams(int paramcount) { paramCount = paramcount; }
  void setLocalVar(VarDecl *var) { localVars.push_back(var); }
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
  void translate(LLVMModuleRef &modRef);
};

class BIRPackage : public BIRNode {
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
  BIRPackage();
  BIRPackage(string orgName, string pkgName, string verName,
             string srcFileName);
  ~BIRPackage();

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
  void translate(LLVMModuleRef &modRef);
};

#endif // BIR_H
