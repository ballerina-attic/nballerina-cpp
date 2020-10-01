#ifndef BIRREADER_H
#define BIRREADER_H

#include <fstream>
#include "BIR.h"

class constant_pool_set_t;
class constant_pool_entry_t;
class package_cp_info_t;
class shape_cp_info_t;
class string_cp_info_t;

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

class int_cp_info_t : public constant_pool_entry_t {

    public:
        int_cp_info_t() { }
        void _read();
        ~int_cp_info_t();

    private:
        int64_t m_value;

    public:
        int64_t value() const { return m_value; }
};


class constant_pool_set_t {

  public:
      constant_pool_set_t() { }
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
