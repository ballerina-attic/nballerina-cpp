#ifndef BIR_H
#define BIR_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm-c/Core.h"

#define DEFAULT_VERSION 0
using namespace std;
using namespace llvm;

// Forward Declarations
class TypeDecl;
class BasicBlock;
class VarDecl;
class BIRFunction;
class NonTerminatorInsn;

enum SymbolKind {
	LOCAL_SYMBOL_KIND,
	GLOBAL_SYMBOL_KIND
};

enum VarKind {
	LOCAL_VAR_KIND = 1,
	TEMP_VAR_KIND, 
	RETURN_VAR_KIND, 
	ARG_VAR_KIND, 
	GLOBAL_VAR_KIND, 
	SELF_VAR_KIND, 
	CONSTANT_VAR_KIND
};

enum InstructionKind {
	BINARY_ADD = 1,
	BINARY_SUB = 2,
	BINARY_MUL = 3,
	BINARY_DIV = 4,
	BINARY_MOD = 5,
	BINARY_EQUAL = 6,
	BINARY_NOT_EQUAL = 7,
	BINARY_GREATER_THAN = 8,
	BINARY_GREATER_EQUAL = 9,
	BINARY_LESS_THAN = 10,
	BINARY_LESS_EQUAL = 11,
	BINARY_REF_EQUAL = 12,
	BINARY_REF_NOT_EQUAL = 13,
	BINARY_CLOSED_RANGE = 14,
	BINARY_HALF_OPEN_RANGE = 15,
	BINARY_ANNOT_ACCESS = 16,
	BINARY_BITWISE_AND = 17,
	BINARY_BITWISE_OR = 18,
	BINARY_BITWISE_XOR = 19,
	BINARY_BITWISE_LEFT_SHIFT = 20,
	BINARY_BITWISE_RIGHT_SHIFT = 21,
	BINARY_BITWISE_UNSIGNED_RIGHT_SHIFT = 22,
	INS_KIND_MOVE=23, 
	INS_KIND_CONST_LOAD=24, 
	INS_KIND_NEW_MAP=25, 
	INS_KIND_NEW_INST=26
}; // we have to add more from bir-model.bal file

enum VarScope {
	VAR_SCOPE_GLOBAL=1, 
	VAR_SCOPE_FUNCTION=2
};

// Forward Declarations
class TypeDecl;
class Scope;

class Location {
  private:
    string  fileName;
    int     lineNum;
    int     columnNum;

  public:
    Location();
    Location(string name, int line, int col);
    ~Location();

    string getFileName();
    int    getLineNum();
    int    getColumnNum();

    virtual void translate();
};

class BIRNode {
  private:
    Location  *loc;

  public:
    BIRNode ();
    BIRNode (Location  *pos);
    ~BIRNode ();

    Location * getLocation()                 { return loc; }
    void       setLocation(Location *newLoc) { loc = newLoc; }

    virtual void translate(LLVMModuleRef modRef);
};

class PackageID {
  private:
    string        orgName;
    string        sourceFileName;   // name is actually the source file name
    string        version;
    bool          isUnnamed = false;
    list<string>  nameComps;

  public:
    PackageID();
    PackageID(string orgName, list<string> nameComps, string version);
    PackageID(string orgName, string name, string version);
    PackageID(string sourceFileName);
    ~PackageID();
    
    string getOrgName()                 { return orgName; }
    string getSourceFileName()          { return sourceFileName; }
    string getVersion()                 { return version; }
    bool   isUnnamed()                  { return isUnnamed; }
    void   addNameComps(string name)    { nameComps.push_back(name); }
    string getFirstNameComp()           { return nameComps.front(); }
    string getLastNameComp()            { return nameComps.back(); }
    bool   nameCompExists(string name);

    const_iterator getCompName(string name);  // Return the iterator to the entry matching name

    virtual void translate();
};

class Symbol {
  private:
    int         tag;
    int         flags;
    string      name;
    PackageID  *pkgID;
    SymbolKind  kind;
    TypeDecl   *type;
    Symbol     *owner;
    bool        tainted;
    bool        closure;
    // MarkdownDocAttachment markdownDocumentation;
    Scope      *scope;

  public:
    Symbol();
    Symbol(int tag, int flags, string name, PackageID *pkgID, TypeDecl *type,
           Symbol *owner);
    ~Symbol();

    int          getTag()               { return tag; }
    int          getFlags()             { return flags; }
    string       getName()              { return name; }
    PackageID *  getPackageID()         { return pkgID; }
    SymbolKind * getSymbolKind()        { return kind; }
    TypeDecl *   getTypeDecl()          { return type; }
    Symbol *     getOwner()             { return owner; }
    bool         isTainted()            { return tainted; }
    bool         isClosure()            { return closure; }
    Scope *      getScope()             { return scope; }

    virtual void translate();
};

class ScopeEntry {
  private:
    Symbol     *symbol;
    ScopeEntry *next;

  public:
    ScopeEntry(Symbol *sym, ScopeEntry *next);
    ~ScopeEntry();

    Symbol *     getSymbol()    { return symbol; }
    ScopeEntry * getNextScope() { return next; }
};

class Scope {
  private:
    //static final int        DEFAULT_SIZE = 10;
    //static final ScopeEntry NOT_FOUND_ENTRY = new ScopeEntry(NULL, NULL);
    Symbol                  *owner;
    map<string, ScopeEntry *>  entries;

  public:
    Scope();
    Scope(Symbol owner);
    Scope(Symbol owner, map<string, ScopeEntry> entries);
    ~Scope();

    Symbol * getOwner() { return owner; }
    void     addNewScopeEntry(string name, ScopeEntry *entry)
             { entries.insert(std::pair<string, ScopeEntry *>(name, entry)); }

    // Implement using std::map::find() 
    ScopeEntry * getScopeEntry(string name);

    virtual void translate();
};


class TypeSymbol: public Symbol {
  private:
    bool        isLabel;

  public:
    TypeSymbol();
    TypeSymbol(int symTag, int flags, string name, PackageID pkgID,
               TypeDecl type, Symbol owner);
    ~TypeSymbol();

    bool isLabel() { return isLabel; }
};

class TypeDecl {
  private:
    int         tag;
    TypeSymbol *typeSym;
    string      name;
    int         flags;

  public:
    TypeDecl();
    TypeDecl(int tag, TypeSymbol tsymbol);
    TypeDecl(int tag, TypeSymbol tsymbol, int flags);
    TypeDecl(int tag, TypeSymbol tsymbol, string name, int flags);
    ~TypeDecl();

    int          getTypeTag()           { return tag; }
    string       getTypeDeclName()      { return name; }
    TypeSymbol * getTypeSymbol()        { return typeSym; }
    int          getFlags()             { return flags; }

    virtual void translate();
};

class Operand: public BIRNode {
  private:
    VarDecl   *varDecl;

  public:
    Operand();
    Operand(VarDecl *vDecl);
    ~Operand();

    VarDecl * getVarDecl()                 { return varDecl; }
    void      setVarDecl(VarDecl *newDecl) { varDecl = newDecl; }

    void translate(LLVMModuleRef modRef);
};

class AbstractInsn: public BIRNode {
  private:
    InstructionKind   kind;
    Operand          *lhsOp;

  public:
    AbstractInsn();
    AbstractInsn(Location *pos, InstructionKind kind, Operand *lOp);
    ~AbstractInsn();

    InstructionKind getInstKind()   { return kind; }
    Operand *       getLhsOperand() { return lhsOp; }

    void setInstKind(InstructionKind newKind)   { kind = NewKind; }
    void setLhsOperand(Operand *lOp)            { lhsOp = lOp; }

    void translate(LLVMModuleRef modRef);
};

class NonTerminatorInsn: public AbstractInsn {
  private:
    BIRFunction       *BFunc;

  public:
    NonTerminatorInsn();
    NonTerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp,
                      BIRFunction *func);
    ~NonTerminatorInsn();

    void          setFunction(BIRFunction *func) { BFunc = func; }
    BIRFunction * getFunction()                  { return BFunc; }

    void translate(LLVMModuleRef modRef);
};

class TerminatorInsn: public AbstractInsn {
  private:
    BasicBlock  *thenBB;

  public:
    TerminatorInsn();
    TerminatorInsn(Location *pos, InstructionKind kind, BIRFunction func,
                   BasicBlock *then);
    ~TerminatorInsn();

    BasicBlock * getNextBB                    { return thenBB; }
    void         setNextBB(BasicBlock *block) { thenBB = block; }

    void translate(LLVMModuleRef modRef);
};

class MoveInsn: public NonTerminatorInsn {
  private:
    Operand  *rhsOp;

  public:
    MoveInsn();
    MoveInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp,
             BIRFunction *func);
    MoveInsn(Operand *lOp, Operand *rOp);
    ~MoveInsn();

    Operand * getRhsOp()             { return rhsOp; }
    void      setRhsOp(Operand *rOp) { rhsOp = rOp; }

    void translate(LLVMModuleRef modRef);
};

class ConstantLoadInsn : public NonTerminatorInsn {
  private:
    unsigned long long value;

  public:
    ConstantLoadInsn();
    ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                     unsigned long long val, BIRFunction *func);
    ConstantLoadInsn(Operand *lOp, unsigned long long val);
    ~ConstantLoadInsn();

    unsigned long long getValue()                       { return value; }
    void               setValue(unsigned long long val) { value = val; }

    void translate(LLVMModuleRef modRef);
};

class BinaryOpInsn : public NonTerminatorInsn {
  private:
    Operand  *rhsOp1;
    Operand  *rhsOp2;
    
  public:
    BinaryOpInsn();
    BinaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp1,
                 Operand *rOp2, BIRFunction *func);
    BinaryOpInsn(Operand *lOp, Operand *rOp1, Operand *rOp2);
    ~BinaryOpInsn();

    Operand * getRhsOp1()            { return rhsOp1; }
    Operand * getRhsOp2()            { return rhsOp2; }
    void      setRhsOp1(Operand *op) { rhsOp1 = op; }
    void      setRhsOp2(Operand *op) { rhsOp2 = op; }

    void translate(LLVMModuleRef modRef);
};

class BasicBlock: public BIRNode {
  private:
    string                       id;
    vector<NonTerminatorInsn *>  instructions;
    TerminatorInsn              *terminator;
    LLVMBuilderRef 	        *BRef;
    BIRFunction                 *BFunc;
    BasicBlock                  *nextBB;

  public:
    BasicBlock();
    BasicBlock(string id);
    ~BasicBlock();
    BasicBlock(Location *pos, string id, LLVMBuilderRef *buildRef,
               BIRFunction *birFunc, BasicBlock *bb);

    string           getId()             { return id; }
    TerminatorInsn * getTerminatorInsn() { return terminator; }
    LLVMBuilderRef * getLLVMBuilderRef() { return BRef; }
    BIRFunction    * getBIRFunction()    { return BFunc; }
    BasicBlock     * getNextBB()         { return nextBB; }

    void setId(string newId)                     { id = newId; }
    void setTerminatorInsn(TerminatorInsn *insn) { terminator = insn; }
    void setBIRFunction(BIRFunction *func)       { BFunc = func; }
    void setNextBB(BasicBlock *bb)               { nextBB = bb; }

    void translate(LLVMModuleRef modRef);
};

class VarDecl: public BIRNode {
  private:
    TypeDecl      *type;
    string         VarName;
    string         metaVarName;
    VarKind       *kind;
    VarScope      *scope;
    bool           ignoreVariable;
    BasicBlock    *endBB;
    BasicBlock    *startBB;
    int            insOffset;

  public:
    VarDecl();
    VarDecl(Location *pos, string name, string metaName);
    VarDecl(Location *pos, TypeDecl *ty, string name, string metaName,
            VarKind *k, VarScope *sc, int offset);
    ~VarDecl();

    TypeDecl *   getTypeDecl()      { return type; }
    VarKind  *   getVarKind()       { return kind; }
    VarScope *   getVarScope()      { return scope; }
    BasicBlock * getStartBB()       { return startBB; }
    BasicBlock * getEndBB()         { return endBB; }
    string       getVarName()       { return varName; }
    string       getMetaVarName()   { return metaVarName; }
    int          getInsOffset()     { return insOffset; }
    bool         ignore()           { return ignoreVariable; }

    void setTypeDecl(TypeDecl *newType)  { type = newType; }
    void setVarKind(VarKind  *newKind)   { kind = newKind; }
    void setVarScope(VarScope *newScope) { scope newScope; }
    void setStartBB(BasicBlock *bb)      { startBB = bb; }
    void setEndBB(BasicBlock *bb)        { endBB = bb; }
    void setVarName(string newName)      { varName = newName; }
    void setMetaVarName(string newName)  { metaVarName = newName; }
    void setInsOffset(int offset)        { insOffset = offset; }
    void setIgnore(bool truth)           { ignoreVariable = truth; }

    void translate(LLVMModuleRef modRef);
};

class Param: public BIRNode {
  private:
    string    name;
    int       flags;
    TypeDecl *type;

  public:
    Param();
    Param(Location *pos, string name, int flags);
    ~Param();

    int        getFlags()       { return flags; }
    string     getName()        { return name; }
    TypeDecl * getTypeDecl()    { return type; }

    void setFlags(int newFlags)         { flags = newFlags; }
    void setName(string newName)        { name = newName; }
    void setTypeDecl(TypeDecl *newType) { type = newType; }

    void translate(LLVMModuleRef modRef);
};

class FuncParam: public VarDecl {
  private:
    bool    hasDefaultExpr;

  public:
    FuncParam();
    FuncParam(Location *pos, TypeDecl *ty, string name, string metaName,
              VarKind *k, VarScope *sc, int off, bool hasDefExp);
    ~FuncParam();

    bool hasDefaultValue() { return hasDefaultExpr; }

    void translate(LLVMModuleRef modRef);
};

class InvokableType: public TypeDecl {
  private:
    vector<TypeDecl *>  paramTypes;
    TypeDecl           *restType;
    TypeDecl           *returnType;

  public:
    InvokableType();
    InvokableType(vector<TypeDecl*> paramTy, TypeDecl *restTy, TypeDecl *retTy,
                  TypeSymbol *tSymbol);
    InvokableType(vector<TypeDecl*> paramTy, TypeDecl *retTy, TypeSymbol *tSymbol);
    ~InvokableType();

    TypeDecl * getReturnType()          { return returnType; }
    TypeDecl * getRestType()            { return restType; }
    TypeDecl * getParamType(int i)      { return paramTypes[i]; }

    void setReturnType(TypeDecl *ty)   { returnType = ty; }
    void setRestType(TypeDecl *ty)     { restType = ty; }
    void addParamType(TypeDecl *ty)    { paramTypes.push_back(ty); }

    void translate(LLVMModuleRef modRef);
};

class BIRFunction: public BIRNode {
  private:
    string                 name;
    int                    flags;
    InvokableType         *type;
    vector<Param *>        requiredParams;
    VarDecl               *receiver;
    Param                 *restParam;
    int                    paramCount;
    vector<VarDecl *>      localVars;
    VarDecl               *returnVar;
    vector<BasicBlock *>   basicBlocks;
    string                 workerName;
    LLVMBuilderRef         builder;

    map<FuncParam *, vector<BasicBlock *>>  params;

  public:
    BIRFunction();
    BIRFunction(Location pos, string pname, int pflags, InvokableType *ptype,
                string pworkerName);
    BIRFunction(const BIRFunction &);
    ~BIRFunction();

    string               getName()              { return name; }
    int                  getFlags()             { return flags; }
    InvokableType *      getInvokableType()     { return type; }
    vector<Param *>      getParams()            { return requiredParams; }
    Param *              getParam(int i)        { return requiredParams[i]; }
    VarDecl *            getReceiver()          { return receiver; }
    Param *              getRestParam()         { return restParam; }
    vector<VarDecl *>    getLocalVars()         { return localVars; }
    VarDecl *            getLocalVar(int i)     { return localVars[i]; }
    VarDecl *            getReturnVar()         { return returnVar; }
    vector<BasicBlock *> getBasicBlocks()       { return basicBlocks; }
    BasicBlock *         getBasicBlock(int i)   { return basicBlocks[i]; }
    string               getWorkerName()        { return workerName; }
    LLVMBuilderRef *     getLLVMBuilder()       { return builder; }
    int                  numParams()            { return paramCount; }

    void setName(string newName)            { name = newName; }
    void setFlags(int newFlags)             { flags = newFlags; }
    void setInvokableType(InvokableType *t) { type = t; }
    void setParams(vector<Param *> p)       { requiredParams = p; }
    void setParam(Param * param)            { requiredParams.push_back(param); }
    void setReceiver(VarDecl *var)          { receiver = var; }
    void setRestParam(Param *param)         { restParam = param; }
    void setLocalVars(vector<VarDecl *> l)  { localVars = l; }
    void setLocalVar(VarDecl *var)          { localVars.push_back(var); }
    void setReturnVar(VarDecl *var)         { returnVar = var; }
    void setBasicBlocks(vector<BasicBlock *> b) { basicBlocks = b; }
    void setBasicBlock(BasicBlock *bb)      { basicBlocks.push_back(bb); }
    void setWorkerName(string newName)      { workerName = newName; }
    void setLLVMBuilder(LLVMBuilderRef *b)  { builder = b; }

    LLVMTypeRef *  getLLVMTypeRefOfType(TypeDecl *typeD);
    LLVMValueRef * getNewFunctionRef();
    LLVMValueRef * getLocalVarRefUsingId(string locVar);
    LLVMValueRef * getLocalToTempVar(Operand *operand);
    void           translateFunctionBody(LLVMModuleRef modRef);

    void translate(LLVMModuleRef modRef);
};

class BIRPackage: public BIRNode {
  private:
    string                org;
    string                name;
    string                version;
    string                sourceFileName;
    vector<BIRFunction *> functions;

  public:
    BIRPackage();
    BIRPackage(string orgName, string pkgName, string verName,
               string srcFileName);
    ~BIRPackage();

    string getOrgName()         { return org; }
    string getPackageName()     { return name; }
    string getVersion()         { return version; }
    string getSrcFileName()     { return sourceFileName; }

    vector<BIRFunction *> getFunctions()     { return functions; }
    size_t                numFunctions()     { return functions.size(); }
    BIRFunction *         getFunction(int i) { return functions[i]; }

    void setFunctions(vector<BIRFunction *> f)  { functions = f; }
    void addFunction(BIRFunction * f)           { functions.push_back(f); }

    void translate(LLVMModuleRef modRef);
};

#endif // BIR_H
