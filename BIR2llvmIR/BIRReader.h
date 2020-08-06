#ifndef BIRREADER_H
#define BIRREADER_H

#include <istream>
#include <string>
#include <vector>
#include "bir.h"

use namespace std;
use std::string;
use std::vec;


class BIRReader {
  private:
  istream *is;
  string   filename;

  public:
    BIRReader();
    BIRReader(string name); // This should open the file and initialize the
                            // istream
    ~BIRReader();

    string name()  { return filename; }
    char readByte();
    void deserialize(istream *dataInStream) {

        // Read import packages.
        readPackages(dataInStream);

    }

    void readPackages(istream *dataInStream) {

    }

  private:
    BIRFunction readFunction();
    BasicBlock readBasicBlock();
    NonTerminatingInsn readNonTerminatingInsn();
    TerminatingInsn readTerminatingInsn();
    VarDecl readVarDecl();
    Param readParam();
    Symbol readSymbol();
};

struct BinTypeDecl {
    int                    tag;
    struct BinTypeSymbol   typeSym;
    string                 name;
    int                    flags;
};

struct BinTerminatorInsn {
    struct BinInstructionKind   kind;
    struct BinBasicBlock        thenBB;
};

struct BinBasicBlock {
    string                                id;
    vector<struct BinNonTerminatorInsn>   instructions;
    struct BinTerminatorInsn              terminator;
};

struct BinVarDecl {
    struct BinTypeDecl     type;
    string                 name;
    string                 metaVarName;
    struct BinVarKind      kind;
    struct BinVarScope     scope;
    bool                   ignoreVariable;
    struct BinBasicBlock   endBB;
    struct BinBasicBlock   startBB;
    int                    insOffset;
};

struct BinParam {
    string   name;
    int      flags;
};

struct BinInvokableType {
    vector<struct BinTypeDecl> paramTypes;
    TypeDecl                   restType;
    TypeDecl                   returnType;
};

struct BinBIRFunction {
    string                       name;
    string                       workerName;
    int                          flags;
    InvokableType                type;
    vector<struct BinParam>      requiredParams;
    VarDecl                      receiver;
    Param                        restParam;
    int                          paramCount;
    vector<struct BinVarDecl>    localVars;
    VarDecl                      returnVar;
    vector<struct BinBasicBlock> basicBlocks;

    map<FuncParam, vector<BasicBlock>>   params;
    friend istream &operator >> (istream &is, struct BinBIRFunction &readBIRFunction) {
        is >> readBIRFunction.name >> readBIRFunction.workerName >> readBIRFunction.flags >> readBIRFunction.type;
        return is;
    }
};

struct BinBIRPackage {
    string org;
    string name;
    string version;
    string sourceFileName;

    vector<struct BinBIRFunction> functions;
    friend istream &operator >> (istream &is, struct BinBIRPackage &readBIRPackage) {
        is >> readBIRPackage.org >> readBIRPackage.name >> readBIRPackage.version;
        return is;
    }
};

#endif // BIRREADER_H
