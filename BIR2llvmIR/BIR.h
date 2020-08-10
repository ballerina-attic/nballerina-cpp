#ifndef BIR_H
#define BIR_H

#include <string>
#include <vector>
#include <map>
#include <list>

using namespace std;

// Forward Declarations
class TypeDecl;
class BasicBlock;
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
};

class BIRNode {
  private:
    Location  loc;
};

class PackageID {
  private:
    string        orgName;
    string        name;
    string        version = DEFAULT_VERSION;
    bool          isUnnamed = false;
    string        sourceFileName = NULL;
    list<string>  nameComps;

  public:
    PackageID();
    PackageID(string orgName, list<string> nameComps, string version);
    PackageID(string orgName, string name, string version);
    PackageID(string orgName, string sourceFileName, string version);
    PackageID(string sourceFileName);
    ~PackageID();
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
    static final int        DEFAULT_SIZE = 10;
    static final ScopeEntry NOT_FOUND_ENTRY = new ScopeEntry(NULL, NULL);
    Symbol                  owner;
    map<string, ScopeEntry> entries;

  public:
    Scope();
    Scope(Symbol owner);
    Scope(Symbol owner, map<string, ScopeEntry> entries);
    ~Scope();
};

class Symbol {
  private:
    int         tag;
    int         flags;
    string      name;
    PackageID   pkgID;
    SymbolKind  kind;
    TypeDecl    type;
    Symbol      owner;
    bool        tainted;
    bool        closure;
    // MarkdownDocAttachment markdownDocumentation;
    Scope       scope;

  public:
    Symbol();
    Symbol(int tag, int flags, string name, PackageID pkgID, TypeDecl type,
           Symbol owner);
    ~Symbol();
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
};

class Operand: public BIRNode {
  private:
    VarDecl   varDecl;

  public:
    Operand(VarDecl vDecl);
    ~Operand();
};

class AbstractInsn: public BIRNode {
  private:
    InstructionKind   kind;
    Operand           lhsOp;

  public:
    AbstractInsn(Location pos, InstructionKind kind);
    ~AbstractInsn();
};

class NonTerminatorInsn: public AbstractInsn {
  public:
    NonTerminatorInsn(Location pos, InstructionKind kind);
    ~NonTerminatorInsn();
};

class TerminatorInsn: public AbstractInsn {
  private:
    InstructionKind   kind;
    BasicBlock        thenBB;

  public:
    TerminatorInsn(Location pos, InstructionKind kind);
    ~TerminatorInsn();
};

class BasicBlock: public BIRNode {
  private:
    string                    id;
    vector<NonTerminatorInsn> instructions;
    TerminatorInsn            terminator;

  public:
    BasicBlock(string id);
    ~BasicBlock();
};

class VarDecl: public BIRNode {
  private:
    TypeDecl      type;
    string        name;
    string        metaVarName;
    VarKind       kind;
    VarScope      scope;
    bool          ignoreVariable;
    BasicBlock    endBB;
    BasicBlock    startBB;
    int           insOffset;

  public:
    VarDecl();
    VarDecl(Location pos, TypeDecl type, string name, VarScope scope,
            VarKind kind, string metaVarName);
    ~VarDecl();
};

class Param: public BIRNode {
  private:
    string    name;
    int       flags;

  public:
    Param();
    Param(Location pos, string name, int flags);
    ~Param();
};

class FuncParam: public VarDecl {
  private:
    bool    hasDefaultExpr;

  public:
    FuncParam();
    FuncParam(Location pos, TypeDecl type, string name, VarScope scope,
              VarKind kind, string metaVarName, bool hasDefExp);
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
    vector<BasicBlock>  basicBlocks;
    string              workerName;

    map<FuncParam, vector<BasicBlock>>   params;

  public:
    BIRFunction();
    BIRFunction(Location pos, string name, int flags, InvokableType type,
                string workerName);
    BIRFunction(BIRFunction);
    ~BIRFunction();
};

class BIRPackage: public BIRNode {
  private:
    string org;
    string name;
    string version;
    string sourceFileName;

    vector<BIRFunction> functions;

  public:
    BIRPackage();
    BIRPackage(string orgName, string pkgName, string verName,
               string srcFileName);
    ~BIRPackage();
};

#endif // BIR_H
