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

enum instruction_kind_enum_t {
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_GOTO = 1,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_CALL = 2,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_BRANCH = 3,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_RETURN = 4,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_ASYNC_CALL = 5,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_WAIT = 6,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_FP_CALL = 7,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_WK_RECEIVE = 8,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_WK_SEND = 9,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_FLUSH = 10,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_LOCK = 11,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_FIELD_LOCK = 12,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_UNLOCK = 13,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_WAIT_ALL = 14,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_MOVE = 20,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_CONST_LOAD = 21,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_STRUCTURE = 22,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_MAP_STORE = 23,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_MAP_LOAD = 24,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_ARRAY = 25,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_ARRAY_STORE = 26,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_ARRAY_LOAD = 27,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_ERROR = 28,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_TYPE_CAST = 29,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_IS_LIKE = 30,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_TYPE_TEST = 31,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_INSTANCE = 32,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_OBJECT_STORE = 33,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_OBJECT_LOAD = 34,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_PANIC = 35,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_FP_LOAD = 36,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_STRING_LOAD = 37,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_XML_ELEMENT = 38,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_XML_TEXT = 39,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_XML_COMMENT = 40,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_XML_PI = 41,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_XML_SEQ = 42,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_XML_QNAME = 43,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_STRING_XML_QNAME = 44,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_XML_SEQ_STORE = 45,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_XML_SEQ_LOAD = 46,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_XML_LOAD = 47,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_XML_LOAD_ALL = 48,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_XML_ATTRIBUTE_LOAD = 49,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_XML_ATTRIBUTE_STORE = 50,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_TABLE = 51,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_TYPEDESC = 52,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_STREAM = 53,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_TABLE_STORE = 54,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_TABLE_LOAD = 55,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_ADD = 61,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_SUB = 62,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_MUL = 63,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_DIV = 64,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_MOD = 65,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_EQUAL = 66,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NOT_EQUAL = 67,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_GREATER_THAN = 68,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_GREATER_EQUAL = 69,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_LESS_THAN = 70,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_LESS_EQUAL = 71,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_AND = 72,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_OR = 73,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_REF_EQUAL = 74,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_REF_NOT_EQUAL = 75,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_CLOSED_RANGE = 76,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_HALF_OPEN_RANGE = 77,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_ANNOT_ACCESS = 78,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_TYPEOF = 80,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NOT = 81,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEGATE = 82,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_BITWISE_AND = 83,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_BITWISE_OR = 84,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_BITWISE_XOR = 85,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_BITWISE_LEFT_SHIFT = 86,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_BITWISE_RIGHT_SHIFT = 87,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_BITWISE_UNSIGNED_RIGHT_SHIFT = 88,
    INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_PLATFORM = 128
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

void read_typedesc ()
{
      uint8_t ignoredVar = read_u1();
      cout << "\nignoredVar = " << ignoredVar;

      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;

      uint8_t scope = read_u1();
      cout << "\nscope = " << (int)scope;

      uint32_t varDclNameCpIndex = read_s4be();
      cout << "\nvarDclNameCpIndex = " << varDclNameCpIndex;

      uint32_t typeCpIndex = read_s4be();
      cout << "\ntypeCpIndex = " << typeCpIndex;
}

void read_structure ()
{

      uint8_t ignoredVar = read_u1();
      cout << "\nignoredVar = " << ignoredVar;

      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;

      uint8_t scope = read_u1();
      cout << "\nscope = " << (int)scope;

      uint32_t varDclNameCpIndex = read_s4be();
      cout << "\nvarDclNameCpIndex = " << varDclNameCpIndex;

      uint8_t ignoredVar1 = read_u1();
      cout << "\nignoredVar1 = " << ignoredVar1;

      uint8_t kind1 = read_u1();
      cout << "\nkind1 = " << (int)kind1;

      uint8_t scope1 = read_u1();
      cout << "\nscope1 = " << (int)scope1;

      uint32_t varDclNameCpIndex1 = read_s4be();
      cout << "\nvarDclNameCpIndex1 = " << varDclNameCpIndex1;

      uint32_t packageIndex1 = read_s4be();
      cout << "\npackageIndex1 = " << packageIndex1;

      uint32_t typeCpIndex1 = read_s4be();
      cout << "\ntypeCpIndex1 = " << typeCpIndex1;
}

void read_const ()
{
      uint32_t typeCpIndex1 = read_s4be();
      cout << "\ntypeCpIndex1 = " << typeCpIndex1;

      uint8_t ignoredVar = read_u1();
      cout << "\nignoredVar = " << ignoredVar;

      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;

      uint8_t scope = read_u1();
      cout << "\nscope = " << (int)scope;

      uint32_t varDclNameCpIndex = read_s4be();
      cout << "\nvarDclNameCpIndex = " << varDclNameCpIndex;
}

void read_goto ()
{
    uint32_t targetBBNameCpIndex = read_s4be();
    cout << "\ntargetBBNameCpIndex = " << targetBBNameCpIndex;
}

void read_function (int i)
{
  uint32_t sLine = read_s4be();
  uint32_t eLine = read_s4be();
  uint32_t sCol = read_s4be();
  uint32_t eCol = read_s4be();
  uint32_t sourceFileCpIndex = read_s4be();
  uint32_t nameCpIndex = read_s4be();
  uint32_t workdernameCpIndex = read_s4be();
  uint32_t flags = read_s4be();
  uint32_t typeCpIndex = read_s4be();
  cout << "\nReading function body\n";
  cout << "\nsLine = " << sLine;
  cout << "\neLine = " << eLine;
  cout << "\nsCol = " << sCol;
  cout << "\neCol = " << eCol;
  cout << "\nsourceFileCpIndex = " << sourceFileCpIndex;
  cout << "\nnameCpIndex = " << nameCpIndex;
  cout << "\nworkdernameCpIndex = " << workdernameCpIndex;
  cout << "\nflags = " << flags;
  cout << "\ntypeCpIndex = " << typeCpIndex;

  uint64_t annotation_length = read_s8be();
  cout << "\nannotation_length = " << annotation_length;

  uint32_t annotationAttachments = read_s4be();
  cout << "\nannotationAttachments = " << annotationAttachments;

  uint32_t requiredParamCount = read_s4be();
  cout << "\nrequiredParamCount = " << requiredParamCount;

  uint8_t hasRestParam = read_u1();
  cout << "\nhasRestParam = " << (bool)hasRestParam;

  uint8_t hasReceiver = read_u1();
  cout << "\nhasReceiver = " << (bool)hasReceiver;

  uint64_t taintTableLength = read_s8be();
  cout << "\ntaintTableLength = " << taintTableLength;

  std::vector<char> result(taintTableLength);
  is.read(&result[0], taintTableLength);
  cout << "\nData = " << std::string(result.begin(), result.end());

  uint32_t docLength = read_s4be();
  cout << "\ndocLength = " << docLength;

  std::vector<char> result1(docLength);
  is.read(&result1[0], docLength);
  cout << "\nData = " << std::string(result1.begin(), result1.end());

  uint64_t functionBodyLength = read_s8be();
  cout << "\nfunctionBodyLength = " << functionBodyLength;

  uint32_t argsCount = read_s4be();
  cout << "\nargsCount = " << argsCount;

  uint8_t m_has_return_var = read_u1();
  cout << "\nm_has_return_var = " << (bool)m_has_return_var;

  if (m_has_return_var)
  {
      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;

      uint32_t typeCpIndex = read_s4be();
      cout << "\ntypeCpIndex = " << typeCpIndex;

      uint32_t nameCpIndex = read_s4be();
      cout << "\nnameCpIndex = " << nameCpIndex;
  }

  uint32_t defaultParamValue = read_s4be();
  cout << "\ndefaultParamValue = " << defaultParamValue;

  uint32_t localVarCount = read_s4be();
  cout << "\nlocalVarCount = " << localVarCount;

  for (int i = 0; i < localVarCount; i++)
  {
      cout << "\nLocal variable " << i;
      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;

      uint32_t typeCpIndex = read_s4be();
      cout << "\ntypeCpIndex = " << typeCpIndex;

      uint32_t nameCpIndex = read_s4be();
      cout << "\nnameCpIndex = " << nameCpIndex;

  }

  uint8_t hasDefaultParamBB = read_u1();
  cout << "\nhasDefaultParamBB = " << (int)hasDefaultParamBB;

  uint32_t BBCount = read_s4be();
  cout << "\nBBCount = " << BBCount;

  for (int i = 0; i < BBCount; i++)
  {
    cout << "\nReading " << i << "th basic block\n";
    uint32_t nameCpIndex2 = read_s4be();
    cout << "\nnameCpIndex2 = " << nameCpIndex2;

    uint32_t insnCount = read_s4be();
    cout << "\ninsnCount = " << insnCount;
    cout << "\nReading instructions from BB\n";
    for (int i = 0; i < insnCount; i++)
    {
      cout << "\nReading " << i << "th instruction\n";
      uint32_t sLine = read_s4be();
      uint32_t eLine = read_s4be();
      uint32_t sCol = read_s4be();
      uint32_t eCol = read_s4be();
      uint32_t sourceFileCpIndex = read_s4be();

      cout << "\nsLine = " << sLine;
      cout << "\neLine = " << eLine;
      cout << "\nsCol = " << sCol;
      cout << "\neCol = " << eCol;
      cout << "\nsourceFileCpIndex = " << sourceFileCpIndex;

      uint8_t insnkind = read_u1();
      cout << "\ninsnkind = " << (instruction_kind_enum_t)insnkind;


	  switch ((instruction_kind_enum_t)insnkind) {
	    case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_TYPEDESC: {
		read_typedesc();
		break;
	    }
	    case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_STRUCTURE: {
		read_structure();
		break;
	    }
	    case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_CONST_LOAD: {
		read_const();
		break;
	    }
	    case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_GOTO: {
		read_goto();
		break;
            }
	    case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_RETURN: {
		//read_return();
		break;
	    }
         }
    }
  }

  uint32_t errorEntriesCount = read_s4be();
  cout << "\nerrorEntriesCount = " << errorEntriesCount;

  uint32_t channelsLength = read_s4be();
  cout << "\nchannelsLength = " << channelsLength;
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
    }
};*/

struct BinBIRFunction {
    uint32_t sLine;
    uint32_t eLine;
    uint32_t sCol;
    uint32_t eCol;
    uint32_t sourceFileCpIndex;
    uint32_t nameCpIndex;
    uint32_t workdernameCpIndex;
    uint32_t flags;
    uint32_t typeCpIndex;
    uint64_t annotation_length;
    uint32_t annotationAttachments;
    uint32_t requiredParamCount;
    uint8_t hasRestParam;
    uint8_t hasReceiver;
    uint64_t taintTableLength;
    uint16_t rowCount;
    uint16_t columnCount;
    uint32_t docLength;
    uint8_t hasDoc;
    uint64_t functionBodyLength;
    uint32_t argsCount;
    uint8_t m_has_return_var;
    uint8_t m_kind;
    uint32_t m_typeCpIndex;
    uint32_t m_nameCpIndex;
    uint32_t defaultParamValue;
    uint32_t localVarCount;
    //vector<struct BinVarDecl> local_vars;
    uint8_t hasDefaultParamBB;
    uint32_t BBCount;
    //vector<struct BinBasicBlock> basic_blocks;
    uint32_t errorEntriesCount;
    uint32_t channelsLength;
};

struct BinBIRPackage {
    int32_t idCpIndex;
    int32_t importCount;
    int32_t constCount;
    int32_t typeDefCount;
    int32_t globalVarCount;
    int32_t funcCount;

    vector<struct BinBIRFunction> functions;
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

        // Read module

	readBIRPackage.idCpIndex = read_s4be();
	readBIRPackage.importCount = read_s4be();
	readBIRPackage.constCount = read_s4be();
	readBIRPackage.typeDefCount = read_s4be();
	readBIRPackage.globalVarCount = read_s4be();
	if (readBIRPackage.globalVarCount > 0)
	{
            //Read and ignore Global Var Data i.e. 18 bytes
	    std::vector<char> result(18);
	    is.read(&result[0], 18);
	    cout << "\nData = " << std::string(result.begin(), result.end());
        }

	int32_t typeDefBodyCount = read_s4be();
	cout << "\ntypeDefBodyCount = " << typeDefBodyCount;
	readBIRPackage.funcCount = read_s4be();

	cout << "\nFunc count =" << readBIRPackage.funcCount;

	for (int i = 0; i < readBIRPackage.funcCount; i++) {
	    cout << "\nReading Function = " << i;
	    read_function (i);
        }

	int32_t annotationLength = read_s4be();
	cout << "\nannotationLength = " << annotationLength;

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
