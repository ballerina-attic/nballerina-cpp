#ifndef BIRREADER_H
#define BIRREADER_H

#include<iostream>
#include<string>
#include<fstream>
#include<vector>
using namespace std;
//#include "BIR.h"

enum tag_enum_t {
    TAG_ENUM_CP_ENTRY_INTEGER = 1,
    TAG_ENUM_CP_ENTRY_FLOAT = 2,
    TAG_ENUM_CP_ENTRY_BOOLEAN = 3,
    TAG_ENUM_CP_ENTRY_STRING = 4,
    TAG_ENUM_CP_ENTRY_PACKAGE = 5,
    TAG_ENUM_CP_ENTRY_BYTE = 6,
    TAG_ENUM_CP_ENTRY_SHAPE = 7
};

std::ifstream is;

// Read 1 byte from the stream
uint8_t read_u1() {
    uint8_t value;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}

// Read 2 bytes from the stream
uint16_t read_s2be ()
{
    uint16_t value;
    uint16_t result;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    char *p = (char *)&value;
    char tmp;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    tmp = p[0];
    p[0] = p[1];
    p[1] = tmp;
#endif
    result = value;
    return result;
}

// Read 4 bytes from the stream
uint32_t read_s4be ()
{
    uint32_t value;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
#if __BYTE_ORDER == __LITTLE_ENDIAN
    uint32_t result = 0;
    result |= (value & 0x000000FF) << 24;
    result |= (value & 0x0000FF00) << 8;
    result |= (value & 0x00FF0000) >> 8;
    result |= (value & 0xFF000000) >> 24;
    value = result;
#endif
    return value;
}

// Read 8 bytes from the stream
uint64_t read_s8be ()
{
    uint64_t value;
    uint64_t result;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
#if __BYTE_ORDER == __LITTLE_ENDIAN
    char *p = (char *)&value;
    char tmp;

    tmp = p[0];
    p[0] = p[7];
    p[7] = tmp;

    tmp = p[1];
    p[1] = p[6];
    p[6] = tmp;

    tmp = p[2];
    p[2] = p[5];
    p[5] = tmp;

    tmp = p[3];
    p[3] = p[4];
    p[4] = tmp;
#endif
    result = value;
    return result;
}

void read_shape ()
{
    int32_t length = read_s4be();
    cout << "\nReading shape pool and shape length, STRLEN = " << length;
    std::vector<char> result(length);
    is.read(&result[0], length);
    cout << "\nData = " << std::string(result.begin(), result.end());
}

void read_package ()
{
  int32_t org = read_s4be();
  int32_t name = read_s4be();
  int32_t version = read_s4be();
  cout << "\nReading package\n";
  cout << "\nOrg = " << org;
  cout << "\nName = " << name;
  cout << "\nVersion = " << version;
}

void read_string ()
{
    uint32_t length = read_s4be();
    cout << "\nReading string pool, STRLEN = " << length;
    std::vector<char> result(length);
    is.read(&result[0], length);
    cout << "\nData = " << std::string(result.begin(), result.end());
}

void read_constant_pool() {
    uint8_t ch = read_u1();
    if (ch == TAG_ENUM_CP_ENTRY_STRING)
        cout << "\nReading the constant pool tag = " << (tag_enum_t)ch;

    switch ((tag_enum_t)ch) {
      case TAG_ENUM_CP_ENTRY_STRING: {
          read_string();
          break;
      }
      case TAG_ENUM_CP_ENTRY_PACKAGE: {
          read_package();
          break;
      }
      case TAG_ENUM_CP_ENTRY_SHAPE: {
          read_shape();
          break;
      }
    }
}

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

struct BinBIRPackage readBIRPackage;

class BIRReader {
  private:
  std::string   filename;

  public:
    std::string name()  { return filename; }
    char readByte();
    void deserialize() {

	is.open("main-bir-dump", ifstream::binary);

        int32_t constant_pool_count;
        constant_pool_count = read_s4be();
        cout << "\nConstant Pool count = " << constant_pool_count;

        for (int i = 0; i < constant_pool_count; i++) {
            cout << "\nConstant pool entry = " << i;
            read_constant_pool ();
        }

        }
        /*void readPackages(dataInStream);

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

    void readFunctions(istream *dataInStream) {
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
