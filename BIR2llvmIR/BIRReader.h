#ifndef BIRREADER_H
#define BIRREADER_H

#include <fstream>
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

        constant_pool_entry_t() {  }

    public:
        /*constant_pool_entry_t* */void _read();

    /*public:
        ~constant_pool_entry_t();*/

    private:
        tag_enum_t m_tag;
        bool n_cp_info;

    public:
        tag_enum_t tag() const { return m_tag; }
};

//std::vector<constant_pool_entry_t*> m_constant_pool_entries;

class string_cp_info_t : public constant_pool_entry_t {

    public:

        string_cp_info_t() {  }

    public:
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


class shape_cp_info_t : public constant_pool_entry_t {

    public:

        shape_cp_info_t() {  }

    public:
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


class package_cp_info_t : public constant_pool_entry_t {

    public:

        package_cp_info_t() { m_org_index=0; m_name_index=0; m_version_index=0; }

    public:
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


class constant_pool_set_t {

  public:

      constant_pool_set_t() {  _read(); }

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

class BIRReader {
  private:
  std::string   filename;

  public:
    std::string name()  { return filename; }
    char readByte();
    void deserialize();
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

  private:
    BIRFunction readFunction();
    BasicBlock readBasicBlock();
    NonTerminatingInsn readNonTerminatingInsn();
    TerminatingInsn readTerminatingInsn();
    VarDecl readVarDecl();
    Param readParam();
    Symbol readSymbol();*/
};

extern uint8_t read_u1();
extern uint16_t read_s2be();
extern uint32_t read_s4be();
extern uint64_t read_s8be();

#endif // BIRREADER_H
