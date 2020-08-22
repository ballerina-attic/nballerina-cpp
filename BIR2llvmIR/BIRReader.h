#ifndef BIRREADER_H
#define BIRREADER_H

#include <istream>
#include<string>
#include "BIR.h"

class BIRReader {
  private:
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
        void readPackages(dataInStream);

	// Read constants.
        void readConstants(dataInStream) {}

        // Read typeDescRef definitions.
        void readtypeDescRef(dataInStream) {}

        // Read package level variables.
        void readVariables(dataInStream) {}

        void readTypeDefBodies(dataInStream) {}

        // Read functions.
        void readFunctions(dataInStream) {}

        // Read annotations.
        void readAnnotations(dataInStream) {}
    }

    void readPackages(istream dataInStream) {
      int symbolCount;
      dataInStream >> symbolCount;
      for (int i = 0; i < symbolCount; i++) {
            dataInStream >> BinBIRPackage;
      }
    }

    /*void readFunctions(istream *dataInStream) {
      int symbolCount;
      dataInStream >> symbolCount;
      for (int i = 0; i < symbolCount; i++) {
            dataInStream >> BinBIRFunction;
      }
    }*/

  private:
    BIRFunction readFunction();
    BasicBlock readBasicBlock();
    NonTerminatingInsn readNonTerminatingInsn();
    TerminatingInsn readTerminatingInsn();
    VarDecl readVarDecl();
    Param readParam();
    Symbol readSymbol();
};
/*
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
};*/

struct BinBIRPackage {
    string org;
    string name;
    string version;
    string sourceFileName;

    //vector<struct BinBIRFunction> functions;
    friend ifstream &operator >> (ifstream &is, struct BinBIRPackage &readBIRPackage) {
        is >> readBIRPackage.org;
	is >> readBIRPackage.name;
	is >> readBIRPackage.version;
        return is;
    }
};

#endif // BIRREADER_H
