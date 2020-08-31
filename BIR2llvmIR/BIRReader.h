#ifndef BIRREADER_H
#define BIRREADER_H

#include<fstream>
#include "BIR.h"

class module_t;
class instruction_t;
class function_t;
class golbal_var_t;
class basic_block_t;
class constant_pool_set_t;
class constant_pool_entry_t;
class package_cp_info_t;
class shape_cp_info_t;
class string_cp_info_t;

enum type_tag_enum_t {
        TYPE_TAG_ENUM_TYPE_TAG_INT = 1,
        TYPE_TAG_ENUM_TYPE_TAG_BYTE = 2,
        TYPE_TAG_ENUM_TYPE_TAG_FLOAT = 3,
        TYPE_TAG_ENUM_TYPE_TAG_DECIMAL = 4,
        TYPE_TAG_ENUM_TYPE_TAG_STRING = 5,
        TYPE_TAG_ENUM_TYPE_TAG_BOOLEAN = 6,
        TYPE_TAG_ENUM_TYPE_TAG_JSON = 7,
        TYPE_TAG_ENUM_TYPE_TAG_XML = 8,
        TYPE_TAG_ENUM_TYPE_TAG_TABLE = 9,
        TYPE_TAG_ENUM_TYPE_TAG_NIL = 10,
        TYPE_TAG_ENUM_TYPE_TAG_ANYDATA = 11,
        TYPE_TAG_ENUM_TYPE_TAG_RECORD = 12,
        TYPE_TAG_ENUM_TYPE_TAG_TYPEDESC = 13,
        TYPE_TAG_ENUM_TYPE_TAG_STREAM = 14,
        TYPE_TAG_ENUM_TYPE_TAG_MAP = 15,
        TYPE_TAG_ENUM_TYPE_TAG_INVOKABLE = 16,
        TYPE_TAG_ENUM_TYPE_TAG_ANY = 17,
        TYPE_TAG_ENUM_TYPE_TAG_ENDPOINT = 18,
        TYPE_TAG_ENUM_TYPE_TAG_ARRAY = 19,
        TYPE_TAG_ENUM_TYPE_TAG_UNION = 20,
        TYPE_TAG_ENUM_TYPE_TAG_INTERSECTION = 21,
        TYPE_TAG_ENUM_TYPE_TAG_PACKAGE = 22,
        TYPE_TAG_ENUM_TYPE_TAG_NONE = 23,
        TYPE_TAG_ENUM_TYPE_TAG_VOID = 24,
        TYPE_TAG_ENUM_TYPE_TAG_XMLNS = 25,
        TYPE_TAG_ENUM_TYPE_TAG_ANNOTATION = 26,
        TYPE_TAG_ENUM_TYPE_TAG_SEMANTIC_ERROR = 27,
        TYPE_TAG_ENUM_TYPE_TAG_ERROR = 28,
        TYPE_TAG_ENUM_TYPE_TAG_ITERATOR = 29,
        TYPE_TAG_ENUM_TYPE_TAG_TUPLE = 30,
        TYPE_TAG_ENUM_TYPE_TAG_FUTURE = 31,
        TYPE_TAG_ENUM_TYPE_TAG_FINITE = 32,
        TYPE_TAG_ENUM_TYPE_TAG_OBJECT = 33,
        TYPE_TAG_ENUM_TYPE_TAG_SERVICE = 34,
        TYPE_TAG_ENUM_TYPE_TAG_BYTE_ARRAY = 35,
        TYPE_TAG_ENUM_TYPE_TAG_FUNCTION_POINTER = 36,
        TYPE_TAG_ENUM_TYPE_TAG_HANDLE = 37,
        TYPE_TAG_ENUM_TYPE_TAG_READONLY = 38,
        TYPE_TAG_ENUM_TYPE_TAG_SIGNED32_INT = 39,
        TYPE_TAG_ENUM_TYPE_TAG_SIGNED16_INT = 40,
        TYPE_TAG_ENUM_TYPE_TAG_SIGNED8_INT = 41,
        TYPE_TAG_ENUM_TYPE_TAG_UNSIGNED32_INT = 42,
        TYPE_TAG_ENUM_TYPE_TAG_UNSIGNED16_INT = 43,
        TYPE_TAG_ENUM_TYPE_TAG_UNSIGNED8_INT = 44,
        TYPE_TAG_ENUM_TYPE_TAG_CHAR_STRING = 45,
        TYPE_TAG_ENUM_TYPE_TAG_XML_ELEMENT = 46,
        TYPE_TAG_ENUM_TYPE_TAG_XML_PI = 47,
        TYPE_TAG_ENUM_TYPE_TAG_XML_COMMENT = 48,
        TYPE_TAG_ENUM_TYPE_TAG_XML_TEXT = 49,
        TYPE_TAG_ENUM_TYPE_TAG_NEVER = 50,
        TYPE_TAG_ENUM_TYPE_TAG_NULL_SET = 51,
        TYPE_TAG_ENUM_TYPE_TAG_PARAMETERIZED_TYPE = 52
    };


class position_t {
public:
        int32_t m_s_line;
        int32_t m_e_line;
        int32_t m_s_col;
        int32_t m_e_col;
        int32_t m_source_file_cp_index;
};

class module_t {
public:
        int32_t m_id_cp_index;
        int32_t m_import_count;
        int32_t m_const_count;
        int32_t m_type_definition_count;
        int32_t m_golbal_var_count;
        int32_t m_type_definition_bodies_count;
        int32_t m_function_count;
        int32_t m_annotations_size;
std::vector<function_t> m_functions;
};

class golbal_var_t {
        int8_t m_kind;
        int32_t m_name_cp_index;
        int32_t m_flags;
        int32_t markdown_length;
        int32_t m_type_cp_index;
};

std::vector<golbal_var_t> m_golbal_vars;

class function_t {
public:
        int32_t m_s_line;
        int32_t m_e_line;
        int32_t m_s_col;
        int32_t m_e_col;
        int32_t m_source_file_cp_index;
        int32_t m_name_cp_index;
        int32_t m_worker_name_cp_index;
        int32_t m_flags;
        int32_t m_type_cp_index;
        int64_t m_annotation_attachments_content_length;
        std::string m_annotation_attachments;
        int32_t m_required_param_count;
        uint8_t m_has_rest_param;
        int32_t m_rest_param_name_cp_index;
        bool n_rest_param_name_cp_index;
        uint8_t m_has_receiver;
        bool n_reciever;
        int64_t m_taint_table_length;
        int64_t m_function_body_length;
        std::string m__raw_taint_table;
        std::string m__raw_function_body;
std::vector<basic_block_t> basic_blocks;;
};

class basic_block_t {
public:
        int32_t m_name_cp_index;
        int32_t m_instructions_count;
std::vector<instruction_t> m_instructions;
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
class instruction_t {
public:
        int32_t m_s_line;
        int32_t m_e_line;
        int32_t m_s_col;
        int32_t m_e_col;
        int32_t m_source_file_cp_index;
        instruction_kind_enum_t m_instruction_kind;
        bool n_instruction_structure;
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

class string_cp_info_t {

    public:

        string_cp_info_t() { _read(); }

    private:
        void _read();

    public:
        ~string_cp_info_t();

    private:
        int32_t m_str_len;
        std::string m_value;

    public:
        int32_t str_len() const { return m_str_len; }
        std::string value() const { return m_value; }
    };


class shape_cp_info_t {

    public:

        shape_cp_info_t() { _read(); }

    private:
        void _read();

    public:
        ~shape_cp_info_t();

    private:
        int32_t m_shape_lenght;
        std::string m_value;

    public:
        int32_t shape_lenght() const { return m_shape_lenght; }
        std::string value() const { return m_value; }
    };


class package_cp_info_t {

    public:

        package_cp_info_t() { _read(); }

    private:
        void _read();

    public:
        ~package_cp_info_t();

    private:
        int32_t m_org_index;
        int32_t m_name_index;
        int32_t m_version_index;

    public:
        int32_t org_index() const { return m_org_index; }
        int32_t name_index() const { return m_name_index; }
        int32_t version_index() const { return m_version_index; }
    };


class constant_pool_entry_t {

    public:

        enum tag_enum_t {
            TAG_ENUM_CP_ENTRY_INTEGER = 1,
            TAG_ENUM_CP_ENTRY_FLOAT = 2,
            TAG_ENUM_CP_ENTRY_BOOLEAN = 3,
            TAG_ENUM_CP_ENTRY_STRING = 4,
            TAG_ENUM_CP_ENTRY_PACKAGE = 5,
            TAG_ENUM_CP_ENTRY_BYTE = 6,
            TAG_ENUM_CP_ENTRY_SHAPE = 7
        };

        constant_pool_entry_t() { _read(); }

    private:
        void _read();

    public:
        ~constant_pool_entry_t();

    private:
        tag_enum_t m_tag;
        bool n_cp_info;

    public:
        tag_enum_t tag() const { return m_tag; }
    };

    class constant_pool_set_t {

    public:

        constant_pool_set_t() { _read(); }

    public:
        void _read();

    public:
        ~constant_pool_set_t();

    private:
        int32_t m_constant_pool_count;
        std::vector<constant_pool_entry_t*>* m_constant_pool_entries;

    public:
        int32_t constant_pool_count() const { return m_constant_pool_count; }
        std::vector<constant_pool_entry_t*>* constant_pool_entries() const { return m_constant_pool_entries; }
    };

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
  class function_t function;
  function.m_s_line = read_s4be();
  uint32_t eLine = read_s4be();
  uint32_t sCol = read_s4be();
  uint32_t eCol = read_s4be();
  uint32_t sourceFileCpIndex = read_s4be();
  uint32_t nameCpIndex = read_s4be();
  uint32_t workdernameCpIndex = read_s4be();
  uint32_t flags = read_s4be();
  uint32_t typeCpIndex = read_s4be();
  cout << "\nReading function body\n";
  cout << "\nfunction.m_position.m_s_line = " << function.m_s_line;
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

void string_cp_info_t::_read() {
    cout << "\n\nReading String\n\n";
    m_str_len = read_s4be();
    cout << "\nReading string pool, STRLEN = " << m_str_len;
    std::vector<char> result(m_str_len);
    is.read(&result[0], m_str_len);
    m_value = std::string(result.begin(), result.end());
    cout << "\nm_value = " << m_value;
}

void shape_cp_info_t::_read() {
    cout << "\n\nReading Shape\n\n";
    m_shape_lenght = read_s4be();
    cout << "\n\nShape length " << m_shape_lenght;
    cout << "\nReading shape pool and shape length, STRLEN = " << m_shape_lenght;
    std::vector<char> result(m_shape_lenght);
    is.read(&result[0], m_shape_lenght);
    cout << "\nData = " << std::string(result.begin(), result.end());
}

void package_cp_info_t::_read() {
    cout << "\n\nReading Package\n\n";
    m_org_index = read_s4be();
    cout << "m_org_index = " << m_org_index;
    m_name_index = read_s4be();
    cout << "m_name_index = " << m_name_index;
    m_version_index = read_s4be();
    cout << "m_version_index " << m_version_index;
}

void constant_pool_entry_t::_read() {
    m_tag = static_cast<constant_pool_entry_t::tag_enum_t>(read_u1());
    n_cp_info = true;
    switch (tag()) {
    case TAG_ENUM_CP_ENTRY_PACKAGE: {
        n_cp_info = false;
        package_cp_info_t *m_cp_info = new package_cp_info_t();
        break;
    }
    case TAG_ENUM_CP_ENTRY_SHAPE: {
        n_cp_info = false;
        shape_cp_info_t *m_cp_info = new shape_cp_info_t();
        break;
    }
    case TAG_ENUM_CP_ENTRY_STRING: {
        n_cp_info = false;
        string_cp_info_t *m_cp_info = new string_cp_info_t();
        break;
    }
    }
}

void constant_pool_set_t::_read() {
    m_constant_pool_count = read_s4be();
    int l_constant_pool_entries = constant_pool_count();
    m_constant_pool_entries = new std::vector<constant_pool_entry_t*>();
    m_constant_pool_entries->reserve(l_constant_pool_entries);
    for (int i = 0; i < l_constant_pool_entries; i++) {
        m_constant_pool_entries->push_back(new constant_pool_entry_t());
        cout << "\nRead Constant Pool entry = " << i;
    }
}

class BIRReader {
  private:
  std::string   filename;

  public:
    std::string name()  { return filename; }
    char readByte();
    void deserialize() {

	is.open("main-bir-dump", ifstream::binary);

        constant_pool_set_t *m_constant_pool = new constant_pool_set_t();

        // Read module
        class module_t readBIRPackage;// = new module_t();
	readBIRPackage.m_id_cp_index = read_s4be();
	readBIRPackage.m_import_count = read_s4be();
	readBIRPackage.m_const_count = read_s4be();
	readBIRPackage.m_type_definition_count = read_s4be();
	readBIRPackage.m_golbal_var_count = read_s4be();
	if (readBIRPackage.m_golbal_var_count > 0)
	{
            //Read and ignore Global Var Data i.e. 18 bytes
	    std::vector<char> result(18);
	    is.read(&result[0], 18);
	    cout << "\nData = " << std::string(result.begin(), result.end());
        }

	readBIRPackage.m_type_definition_bodies_count = read_s4be();
	cout << "\ntypeDefBodyCount = " << readBIRPackage.m_type_definition_bodies_count;
	readBIRPackage.m_function_count = read_s4be();

	cout << "\nFunc count =" << readBIRPackage.m_function_count;

        /*readBIRPackage.m_functions = new std::vector<function_t*>();
        readBIRPackage.m_functions.reserve(readBIRPackage.m_function_count);
        for (int i = 0; i < readBIRPackage.m_function_count; i++) {
          readBIRPackage.m_functions.push_back(new function_t);
        }*/

	for (int i = 0; i < readBIRPackage.m_function_count; i++) {
	    cout << "\nReading Function = " << i;
	    read_function (i);
        }

	readBIRPackage.m_annotations_size = read_s4be();
	cout << "\nannotationLength = " << readBIRPackage.m_annotations_size;

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
