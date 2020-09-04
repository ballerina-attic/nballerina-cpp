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
class BasicBlockT;
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
        INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_GOTO = 1,
        BINARY_SUB,
        BINARY_MUL,
        INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_RETURN,
        BINARY_MOD,
        BINARY_EQUAL,
        BINARY_NOT_EQUAL,
        BINARY_GREATER_THAN,
        BINARY_GREATER_EQUAL,
        BINARY_LESS_THAN,
        BINARY_LESS_EQUAL,
        BINARY_REF_EQUAL,
        BINARY_REF_NOT_EQUAL,
        BINARY_CLOSED_RANGE,
        BINARY_HALF_OPEN_RANGE,
        BINARY_ANNOT_ACCESS,
        BINARY_BITWISE_AND,
        BINARY_BITWISE_OR,
        BINARY_BITWISE_XOR,
        BINARY_BITWISE_LEFT_SHIFT,
        INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_CONST_LOAD,
        INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_STRUCTURE,
        BINARY_BITWISE_UNSIGNED_RIGHT_SHIFT,
        INS_KIND_MOVE, 
        INS_KIND_CONST_LOAD,
        INS_KIND_NEW_MAP, 
        INS_KIND_NEW_INST,
        INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_TYPEDESC = 52,
        BINARY_ADD,
        BINARY_DIV
}; // we have to add more from bir-model.bal file

enum VarScope {
	VAR_SCOPE_GLOBAL=1, 
	VAR_SCOPE_FUNCTION
};

// Forward Declarations
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

    virtual void translate(LLVMModuleRef &modRef);
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

    virtual void translate(LLVMModuleRef &modRef);
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
    bool   getIsUnnamed()                  { return isUnnamed; }
    void   addNameComps(string name)    { nameComps.push_back(name); }
    string getFirstNameComp()           { return nameComps.front(); }
    string getLastNameComp()            { return nameComps.back(); }
    bool   nameCompExists(string name);

    typename list<string>::iterator getCompName(string name);  // Return the iterator to the entry matching name

    virtual void translate(LLVMModuleRef &modRef);
};

class Symbol {
  private:
    int         tag;
    int         flags;
    string      name;
    PackageID  *pkgID;
    SymbolKind *kind;
    TypeDecl   *type;
    Symbol     *owner;
    bool        tainted;
    bool        closure;
    // MarkdownDocAttachment markdownDocumentation;
    Scope      *scope;

  public:
    Symbol();
    Symbol(int tag, int flags, string name, SymbolKind *kind,
	   TypeDecl *type, Symbol *owner, Scope *scope);
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

    virtual void translate(LLVMModuleRef &modRef);
};

class ScopeEntry {
  private:
    Symbol     *symbol;
    ScopeEntry *next;

  public:
    ScopeEntry();
    ScopeEntry(Symbol *sym, ScopeEntry *next);
    ~ScopeEntry();

    Symbol *     getSymbol()    { return symbol; }
    ScopeEntry * getNextScope() { return next; }
};

class Scope {
  private:
    static const int        DEFAULT_SIZE = 10;
    //static final ScopeEntry NOT_FOUND_ENTRY = new ScopeEntry(NULL, NULL);
    Symbol                  *owner;
    map<string, ScopeEntry *>  entries;

  public:
    Scope();
    Scope(Symbol *owner);
    Scope(Symbol *owner, map<string, ScopeEntry*> entries);
    ~Scope();

    Symbol * getOwner() { return owner; }
    void     addNewScopeEntry(string name, ScopeEntry *entry)
             { entries.insert(std::pair<string, ScopeEntry *>(name, entry)); }

    ScopeEntry * getScopeEntry(string name);

    virtual void translate(LLVMModuleRef &modRef);
};


class TypeSymbol: public Symbol {
  private:
    bool        isLabel;

  public:
    TypeSymbol();
    TypeSymbol(int symTag, int flags, string name, SymbolKind *kind,
		TypeDecl *type, Symbol *owner, Scope *scope, bool isLab);
    ~TypeSymbol();

    bool getIsLabel() { return isLabel; }
};

class TypeDecl {
  private:
    int         tag;
    TypeSymbol *typeSym;
    string      name;
    int         flags;

  public:
    TypeDecl();
    TypeDecl(TypeSymbol *tsymbol);
    TypeDecl(int tag, TypeSymbol *tsymbol, int flags);
    TypeDecl(int tag, TypeSymbol *tsymbol, string name, int flags);
    ~TypeDecl();

    void    setTypeDeclName(string namep)    { name = namep; }
    void    setTypeTag (int tagp)            { tag = tagp; }
    void    setTypeSymbol(TypeSymbol *tySym) { typeSym = tySym; }
    void    setFlags(int flag)               { flags = flag; }

    int          getTypeTag()           { return tag; }
    string       getTypeDeclName()      { return name; }
    TypeSymbol * getTypeSymbol()        { return typeSym; }
    int          getFlags()             { return flags; }
    
    virtual void translate(LLVMModuleRef &modRef);
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

    void translate(LLVMModuleRef &modRef);
};

class AbstractInsn: public BIRNode {
  private:
    InstructionKind   kind;
    Operand          *lhsOp;
    BIRFunction       *BFunc;

  public:
    AbstractInsn();
    AbstractInsn(Location *pos, InstructionKind kind, Operand *lOp);
    ~AbstractInsn();

    InstructionKind getInstKind()   { return kind; }
    Operand *       getLhsOperand() { return lhsOp; }

    void          setFunction(BIRFunction *func) { BFunc = func; }
    BIRFunction * getFunction()                  { return BFunc; }

    void setInstKind(InstructionKind newKind)   { kind = newKind; }
    void setLhsOperand(Operand *lOp)            { lhsOp = lOp; }

    void translate(LLVMModuleRef &modRef);
};

class NonTerminatorInsn: public AbstractInsn {
  private:
   // BIRFunction       *BFunc;

  public:
    NonTerminatorInsn();
    NonTerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp);
    ~NonTerminatorInsn();

   // void          setFunction(BIRFunction *func) { BFunc = func; }
   // BIRFunction * getFunction()                  { return BFunc; }

    void translate(LLVMModuleRef &modRef);
};

class TerminatorInsn: public AbstractInsn {
  private:
    BasicBlockT  *thenBB;
    bool         patchRequire;
  public:
    TerminatorInsn();
    TerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp,
				 BasicBlockT *then);
    ~TerminatorInsn();

    BasicBlockT * getNextBB()                    { return thenBB; }
    bool getPatchStatus()                      { return patchRequire; }

    void setPatchStatus(bool patchrequire)     { patchRequire = patchrequire; }
    void         setNextBB(BasicBlockT *block) { thenBB = block; }

    void translate(LLVMModuleRef &modRef);
};

class MoveInsn: public NonTerminatorInsn {
  private:
    Operand  *rhsOp;

  public:
    MoveInsn();
    MoveInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp);
    ~MoveInsn();

    Operand * getRhsOp()             { return rhsOp; }
    void      setRhsOp(Operand *rOp) { rhsOp = rOp; }

    void translate(LLVMModuleRef &modRef);
};

class ConstantLoadInsn : public NonTerminatorInsn {
  private:
    unsigned long long value;

  public:
    ConstantLoadInsn();
    ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                     unsigned long long val);
    ~ConstantLoadInsn();

    unsigned long long getValue()                       { return value; }
    void               setValue(unsigned long long val) { value = val; }

    void translate(LLVMModuleRef &modRef);
};

class BinaryOpInsn : public NonTerminatorInsn {
  private:
    Operand  *rhsOp1;
    Operand  *rhsOp2;
    
  public:
    BinaryOpInsn();
    BinaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp, Operand *rOp1,
                 Operand *rOp2);
    ~BinaryOpInsn();

    Operand * getRhsOp1()            { return rhsOp1; }
    Operand * getRhsOp2()            { return rhsOp2; }
    void      setRhsOp1(Operand *op) { rhsOp1 = op; }
    void      setRhsOp2(Operand *op) { rhsOp2 = op; }

    void translate(LLVMModuleRef &modRef);
};

class GoToInsn : public TerminatorInsn {
  private:

  public:
    GoToInsn();
    GoToInsn(Location *pos, InstructionKind kind, Operand *lOp,
             BasicBlockT *nextBB);
    ~GoToInsn();

    void translate(LLVMModuleRef &modRef);
};

class ReturnInsn : public TerminatorInsn {
  private:

  public:
    ReturnInsn();
    ReturnInsn(Location *pos, InstructionKind kind, Operand *lOp,
		 BasicBlockT *nextBB);
    ~ReturnInsn();

    void translate(LLVMModuleRef &modRef);
};

class BasicBlockT: public BIRNode {
  private:
    string                       id;
    vector<NonTerminatorInsn *>  instructions;
    TerminatorInsn              *terminator;
    LLVMBuilderRef 	        BRef;
    BIRFunction                 *BFunc;
    BasicBlockT                  *nextBB;

  public:
    BasicBlockT();
    BasicBlockT(string id);
    ~BasicBlockT();
    BasicBlockT(Location *pos, string id);

    string           getId()             { return id; }
    TerminatorInsn * getTerminatorInsn() { return terminator; }
    LLVMBuilderRef   getLLVMBuilderRef() { return BRef; }
    BIRFunction    * getBIRFunction()    { return BFunc; }
    BasicBlockT     * getNextBB()         { return nextBB; }
    vector<NonTerminatorInsn *> getNonTerminatorInsn()       { return instructions; }
    NonTerminatorInsn *         getInsn(int i) { return instructions[i]; }

    void setId(string newId)                     { id = newId; }
    void setTerminatorInsn(TerminatorInsn *insn) { terminator = insn; }
    void setLLVMBuilderRef(LLVMBuilderRef buildRef) {BRef = buildRef; } 
    void setBIRFunction(BIRFunction *func)       { BFunc = func; }
    void setNextBB(BasicBlockT *bb)               { nextBB = bb; }
    void addNonTermInsn(NonTerminatorInsn *insn)  { 
			instructions.push_back(insn); }

    void translate(LLVMModuleRef &modRef);
};

class VarDecl: public BIRNode {
  private:
    TypeDecl      *type;
    string         varName;
    string         metaVarName;
    VarKind        kind;
    VarScope      scope;
    bool           ignoreVariable;
    BasicBlockT    *endBB;
    BasicBlockT    *startBB;
    int            insOffset;

  public:
    VarDecl();
    VarDecl(Location *pos, string name, string metaName);
    VarDecl(Location *pos, TypeDecl *ty, string name, string metaName,
            VarKind k, VarScope sc, int offset);
    ~VarDecl();

    TypeDecl *   getTypeDecl()      { return type; }
    VarKind      getVarKind()       { return kind; }
    VarScope     getVarScope()      { return scope; }
    BasicBlockT * getStartBB()       { return startBB; }
    BasicBlockT * getEndBB()         { return endBB; }
    string       getVarName()       { return varName; }
    string       getMetaVarName()   { return metaVarName; }
    int          getInsOffset()     { return insOffset; }
    bool         ignore()           { return ignoreVariable; }

    void setTypeDecl(TypeDecl *newType)  { type = newType; }
    void setVarKind(VarKind  newKind)   { kind = newKind; }
    void setVarScope(VarScope newScope) { scope = newScope; }
    void setStartBB(BasicBlockT *bb)      { startBB = bb; }
    void setEndBB(BasicBlockT *bb)        { endBB = bb; }
    void setVarName(string newName)      { varName = newName; }
    void setMetaVarName(string newName)  { metaVarName = newName; }
    void setInsOffset(int offset)        { insOffset = offset; }
    void setIgnore(bool truth)           { ignoreVariable = truth; }

    void translate(LLVMModuleRef &modRef);
};

class Param: public BIRNode {
  private:
    string    name;
    int       flags;
    TypeDecl *type;

  public:
    Param();
    Param(Location *pos, string name, int flags, TypeDecl *ty);
    ~Param();

    int        getFlags()       { return flags; }
    string     getName()        { return name; }
    TypeDecl * getTypeDecl()    { return type; }

    void setFlags(int newFlags)         { flags = newFlags; }
    void setName(string newName)        { name = newName; }
    void setTypeDecl(TypeDecl *newType) { type = newType; }

    void translate(LLVMModuleRef &modRef);
};

class FuncParam: public VarDecl {
  private:
    bool    hasDefaultExpr;

  public:
    FuncParam();
    FuncParam(Location *pos, TypeDecl *ty, string name, string metaName,
              VarKind k, VarScope sc, int off, bool hasDefExp);
    ~FuncParam();

    bool hasDefaultValue() { return hasDefaultExpr; }

    void translate(LLVMModuleRef &modRef);
};

class InvokableType: public TypeDecl {
  private:
    vector<TypeDecl *>  paramTypes;
    TypeDecl           *restType;
    TypeDecl           *returnType;

  public:
    InvokableType();
    InvokableType(vector<TypeDecl*> paramTy, TypeDecl *restTy, TypeDecl *retTy);
                  /*TypeSymbol *tSymbol);*/
    InvokableType(vector<TypeDecl*> paramTy, TypeDecl *retTy);/* TypeSymbol *tSymbol);*/
    ~InvokableType();

    TypeDecl * getReturnType()          { return returnType; }
    TypeDecl * getRestType()            { return restType; }
    TypeDecl * getParamType(int i)      { return paramTypes[i]; }

    void setReturnType(TypeDecl *ty)   { returnType = ty; }
    void setRestType(TypeDecl *ty)     { restType = ty; }
    void addParamType(TypeDecl *ty)    { paramTypes.push_back(ty); }

    void translate(LLVMModuleRef &modRef);
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
    vector<BasicBlockT *>   basicBlocks;
    string                 workerName;
    LLVMBuilderRef         builder;
    LLVMValueRef           newFunction;
    map<FuncParam *, vector<BasicBlockT *>>  params;
    map<string, LLVMValueRef>          localVarRefs;

  public:
    BIRFunction();
    BIRFunction(Location *pos, string pname, int pflags, InvokableType *ptype,
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
    vector<BasicBlockT *> getBasicBlocks()       { return basicBlocks; }
    BasicBlockT *         getBasicBlock(int i)   { return basicBlocks[i]; }
    string               getWorkerName()        { return workerName; }
    LLVMBuilderRef       getLLVMBuilder()       { return builder; }
    int                  getNumParams()            { return paramCount; }
    LLVMValueRef         getNewFunctionRef()    { return newFunction; }
    map<string , LLVMValueRef>  getLocalVarRefs()  { return localVarRefs; }

    void setName(string newName)            { name = newName; }
    void setFlags(int newFlags)             { flags = newFlags; }
    void setInvokableType(InvokableType *t) { type = t; }
    void setParams(vector<Param *> p)       { requiredParams = p; }
    void setParam(Param * param)            { requiredParams.push_back(param); }
    void setReceiver(VarDecl *var)          { receiver = var; }
    void setRestParam(Param *param)         { restParam = param; }
    void setLocalVars(vector<VarDecl *> l)  { localVars = l; }
    void setNumParams(int paramcount)       { paramCount = paramcount; }
    void setLocalVar(VarDecl *var)          { localVars.push_back(var); }
    void setReturnVar(VarDecl *var)         { returnVar = var; }
    void setBasicBlockTs(vector<BasicBlockT *> b) { basicBlocks = b; }
    void setBasicBlockT(BasicBlockT *bb)      { basicBlocks.push_back(bb); }
    void setWorkerName(string newName)      { workerName = newName; }
    void setLLVMBuilder(LLVMBuilderRef b)  { builder = b; }
    void setLocalVarRefs(map<string, LLVMValueRef> newLocalVarRefs)  {
					localVarRefs = newLocalVarRefs; } 
    void setNewFunctionRef(LLVMValueRef newFuncRef) { newFunction = newFuncRef; }

    LLVMTypeRef   getLLVMTypeRefOfType(TypeDecl *typeD);
    LLVMValueRef  getLocalVarRefUsingId(string locVar);
    LLVMValueRef  getLocalToTempVar(Operand *operand);
    void          translateFunctionBody(LLVMModuleRef &modRef);
    void	  patchInsn(Function* llvnFun);

    void translate(LLVMModuleRef &modRef);
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

    void setOrgName(string orgName)         {  org = orgName; }
    void setPackageName(string pkgName)     { name = pkgName; }
    void setVersion(string verName)         { version = verName; }
    void setSrcFileName(string srcFileName) { sourceFileName = srcFileName; }

    vector<BIRFunction *> getFunctions()     { return functions; }
    size_t                numFunctions()     { return functions.size(); }
    BIRFunction *         getFunction(int i) { return functions[i]; }

    void setFunctions(vector<BIRFunction *> f)  { functions = f; }
    void addFunction(BIRFunction * f)           { functions.push_back(f); }

    void translate();
};

#endif // BIR_H
