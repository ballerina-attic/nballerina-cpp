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

#define DEFAULT_VERSION 0
using namespace std;
using namespace llvm;
// Forward Declarations
class TypeDecl;
class BasicBlock1;
class VarDecl;

class Location {
  private:
    string  fileName;
    int     lineNum;
    int     columnNum;

  public:
    Location();
    Location(string name, int line, int col);
    ~Location();

    string file() { return fileName; }
    int    line() { return lineNum; }
    int    column() { return columnNum; }

    virtual void translate();
};

class BIRNode {
  private:
    Location  loc;

  public:
    BIRNode ();
    BIRNode (Location  local);
    ~BIRNode ();
    virtual void translate(string modRef);
};

class PackageID {
  private:
    string        orgName;
    string        name;
    string        version;
    bool          isUnnamed = false;
    string        sourceFileName;
    list<string>  nameComps;

  public:
    PackageID();
    PackageID(string orgName, list<string> nameComps, string version);
    PackageID(string orgName, string name, string version);
    //PackageID(string orgName, string sourceFileName, string version);
    PackageID(string sourceFileName);
    ~PackageID();

    virtual void translate();
};

class Symbol {
  private:
    int         tag;
    int         flags;
    string      name;
    PackageID   pkgID;
    //SymbolKind  kind;
    //TypeDecl    type;
    //Symbol      owner;
    bool        tainted;
    bool        closure;
    // MarkdownDocAttachment markdownDocumentation;
    //Scope       scope;

  public:
    Symbol();
    Symbol(int tag, int flags, string name, PackageID pkgID,// TypeDecl type,
           Symbol owner);
    ~Symbol();

    virtual void translate();
};

class ScopeEntry {
  private:
    Symbol      symbol;
    ScopeEntry *next;

  public:
    ScopeEntry(Symbol sym, ScopeEntry *next);
    ~ScopeEntry();
};

class Scope {
  private:
    //static final int        DEFAULT_SIZE = 10;
    //static final ScopeEntry NOT_FOUND_ENTRY = new ScopeEntry(NULL, NULL);
    Symbol                  owner;
    map<string, ScopeEntry> entries;

  public:
    Scope();
    Scope(Symbol owner);
    Scope(Symbol owner, map<string, ScopeEntry> entries);
    ~Scope();

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
};

class TypeDecl {
  private:
    int         tag;
    TypeSymbol  typeSym;
    string      name;
    int         flags;

  public:
    TypeDecl();
    TypeDecl(int tag, TypeSymbol tsymbol);
    TypeDecl(int tag, TypeSymbol tsymbol, int flags);
    TypeDecl(int tag, TypeSymbol tsymbol, string name, int flags);
    ~TypeDecl();

    virtual void translate();
};

class Operand: public BIRNode {
  private:
    //VarDecl   varDecl;

  public:
    Operand();//VarDecl vDecl);
    ~Operand();
    void translate();
};

class AbstractInsn: public BIRNode {
  private:
    //InstructionKind   kind;
    Operand           lhsOp;

  public:
    AbstractInsn();
    AbstractInsn(Location pos);//, InstructionKind kind);
    ~AbstractInsn();
    void translate();
};

class NonTerminatorInsn: public AbstractInsn {
  public:
    NonTerminatorInsn();
    NonTerminatorInsn(Location posi);//, InstructionKind kind);
    ~NonTerminatorInsn();
};

class TerminatorInsn: public AbstractInsn {
  private:
   // InstructionKind   kind;
   // BasicBlock1        thenBB;

  public:
    TerminatorInsn();
    TerminatorInsn(Location pos);//, InstructionKind kind);
    ~TerminatorInsn();
    void translate();
};

class BasicBlock1: public BIRNode {
  private:
    string                    id;
    vector<NonTerminatorInsn> instructions;
    TerminatorInsn            terminator;

  public:
    BasicBlock1();
    BasicBlock1(string id);
    ~BasicBlock1();
    void translate();
};

class VarDecl: public BIRNode {
  private:
    TypeDecl      type;
    string        name;
    string        metaVarName;
    //VarKind       kind;
    //VarScope      scope;
    bool          ignoreVariable;
    BasicBlock1    endBB;
    BasicBlock1    startBB;
    int           insOffset;

  public:
    VarDecl();
    VarDecl(string name, string metaVarName);
    ~VarDecl();
    void translate();
};

class Param: public BIRNode {
  private:
    string    name;
    int       flags;

  public:
    Param();
    Param(Location pos, string name, int flags);
    ~Param();
    void translate();
};

class FuncParam: public VarDecl {
  private:
    bool    hasDefaultExpr;

  public:
    FuncParam();
    FuncParam(Location pos, TypeDecl type, string name, //VarScope scope,
              /*VarKind kind,*/ string metaVarName, bool hasDefExp);
    ~FuncParam();

};

class InvokableType: public TypeDecl {
  private:
    vector<TypeDecl>  paramTypes;
    TypeDecl          restType;
    TypeDecl          returnType;

  public:
    InvokableType();
    InvokableType(vector<TypeDecl> paramTy, TypeDecl restTy, TypeDecl retTy,
                  TypeSymbol tSymbol);
    InvokableType(vector<TypeDecl> paramTy, TypeDecl retTy, TypeSymbol tSymbol);
    ~InvokableType();
};

class BIRFunction: public BIRNode {
  private:
    string              name;
    int                 flags;
    InvokableType       type;
    vector<Param>       requiredParams;
    VarDecl             receiver;
    Param               restParam;
    int                 paramCount;
    vector<VarDecl>     localVars;
    VarDecl             returnVar;
    vector<BasicBlock1>  basicBlocks;
    string              workerName;

    map<FuncParam, vector<BasicBlock1>>   params;

  public:
    BIRFunction();
    BIRFunction(Location pos, string pname, int pflags, InvokableType ptype,
                string pworkerName);
    BIRFunction(const BIRFunction&);
    ~BIRFunction();
    void translate(string modRef);
};

class BIRPackage: public BIRNode {
  private:
    string org;
    string name;
    string version;
    string sourceFileName;
  public:
    vector<BIRFunction*> functions;

  public:
    BIRPackage();
    BIRPackage(string orgName, string pkgName, string verName,
               string srcFileName);
    ~BIRPackage();
    void translate(string modRef); // We have to Pass ModuleRef
};

#endif // BIR_H
