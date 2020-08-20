#ifndef BIRREADER_H
#define BIRREADER_H

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
using namespace std;
//#include "BIR.h"

struct BinBIRPackage readBIRPackage;

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
    std::string org;
    std::string name;
    std::string version;
    std::string sourceFileName;

    //vector<struct BinBIRFunction> functions;
};

class BIRReader {
  private:
  std::string   filename;

  public:
    BIRReader();
    BIRReader(std::string name); // This should open the file and initialize the
                            // istream
    ~BIRReader();

    std::string name()  { return filename; }
    char readByte();
    void deserialize() {

        // Code copied from bir_t::constant_pool_set_t::_read() L90
        // Purpose - read total pool count
        m_constant_pool_count = m__io->read_s4be();
        int l_constant_pool_entries = constant_pool_count();
        m_constant_pool_entries = new std::vector<constant_pool_entry_t*>();
        m_constant_pool_entries->reserve(l_constant_pool_entries);
        for (int i = 0; i < l_constant_pool_entries; i++) {
            m_constant_pool_entries->push_back(new constant_pool_entry_t(m__io, this, m__root));
        }

        ifstream dataInStream;
        int size = 0;
        dataInStream.open("main-bir-dump", ios::in|ios::binary);
        dataInStream.seekg(0, ios::end);
        size = (int)dataInStream.tellg();
        dataInStream.seekg (0, ios::beg);

        // Read import packages.

        readBIRPackage.org = m__io->read_s4be();
        readBIRPackage.name = m__io->read_s4be();
        readBIRPackage.value = m__io->read_s4be();
        //void readPackages(dataInStream);

        // Read constants.
        void readConstants(dataInStream);

        // Read typeDescRef definitions.
        void readtypeDescRef(dataInStream);

        // Read package level variables.
        void readVariables(dataInStream);

        void readTypeDefBodies(dataInStream);

        // Read functions.
        void readFunctions(dataInStream);

        // Read annotations.
        void readAnnotations(dataInStream);
    }

    void readPackages(ifstream dataInStream) {
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
    }

  private:
    BIRFunction readFunction();
    BasicBlock readBasicBlock();
    NonTerminatingInsn readNonTerminatingInsn();
    TerminatingInsn readTerminatingInsn();
    VarDecl readVarDecl();
    Param readParam();
    Symbol readSymbol();*/
};

#endif // BIRREADER_H
