#ifndef BIRREADER_H
#define BIRREADER_H

#include <fstream>
#include "BIR.h"

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
        void _read();

    private:
        tag_enum_t m_tag;
        bool n_cp_info;

    public:
        tag_enum_t tag() const { return m_tag; }
};

class string_cp_info_t : public constant_pool_entry_t {

    public:
        string_cp_info_t() {  }
        void _read();
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
        void _read();
        ~shape_cp_info_t();

    private:
        int32_t m_shape_lenght;
        std::string m_value;
        type_tag_enum_t m_type_tag;
        int32_t m_name_index;
        int32_t m_type_flag;
        int32_t m_type_special_flag;
        int32_t m_param_count;
        uint8_t m_has_rest_type;
        int32_t m_return_type_index;

    public:
        int32_t shape_lenght() const { return m_shape_lenght; }
        std::string value() const { return m_value; }
        type_tag_enum_t type_tag() const { return m_type_tag; }
        int32_t name_index() const { return m_name_index; }
        int32_t type_flag() const { return m_type_flag; }
        int32_t type_special_flag() const { return m_type_special_flag; }
        int32_t param_count() const { return m_param_count; }
        uint8_t has_rest_type() const { return m_has_rest_type; }
        int32_t return_type_index() const { return m_return_type_index; }
};


class package_cp_info_t : public constant_pool_entry_t {

    public:
        package_cp_info_t() { m_org_index = 0; m_name_index = 0; m_version_index = 0; }
        void _read();
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
      void _read();
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
    BIRReader(std::string FileName) { filename = FileName; }
    void setFileName(std::string FileName) { filename = FileName; }
    std::string name()  { return filename; }
    BIRPackage* deserialize(BIRPackage *);
};

extern uint8_t read_u1();
extern uint16_t read_s2be();
extern uint32_t read_s4be();
extern uint64_t read_s8be();

#endif // BIRREADER_H
