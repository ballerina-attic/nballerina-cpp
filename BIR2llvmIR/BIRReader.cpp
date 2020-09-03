#include "BIR.h"
#include "BIRReader.h"
using namespace std;

std::ifstream is;

// Read 1 byte from the stream
uint8_t read_u1() {
    uint8_t value;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}

// Read 1 byte from the stream but do not move pointer
uint8_t peek_read_u1() {
    uint8_t value;
    value = is.peek();
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

void read_function (int i, constant_pool_set_t *m_constant_pool)
{
  class function_t function;
  function.m_s_line = read_s4be();
  uint32_t eLine = read_s4be();
  uint32_t sCol = read_s4be();
  uint32_t eCol = read_s4be();
  uint32_t sourceFileCpIndex = read_s4be();
  uint32_t nameCpIndex = read_s4be();
  if (i == 3)
  {
	constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(nameCpIndex);
	string_cp_info_t *version_pointer2 = static_cast<string_cp_info_t *> (entry_pointer);
	cout << "\nFunction name = " << version_pointer2->value(); 
  }
  uint32_t workdernameCpIndex = read_s4be();
  uint32_t flags = read_s4be();
  if (i == 3)
  {
        constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(flags);
        string_cp_info_t *version_pointer2 = static_cast<string_cp_info_t *> (entry_pointer);
        cout << "\nflags = " << version_pointer2->value();
  }
  uint32_t typeCpIndex = read_s4be();
  cout << "\nReading function body\n";
  cout << "\nfunction.m_position.m_s_line = " << function.m_s_line;
  cout << "\neLine = " << eLine;
  cout << "\nsCol = " << sCol;
  cout << "\neCol = " << eCol;
  cout << "\nsourceFileCpIndex = " << sourceFileCpIndex;
  cout << "\nnameCpIndex = " << nameCpIndex;
  cout << "\nworkdernameCpIndex = " << workdernameCpIndex;
  if (i == 3)
  {
        constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(workdernameCpIndex);
        string_cp_info_t *version_pointer2 = static_cast<string_cp_info_t *> (entry_pointer);
        cout << "\nWorker name = " << version_pointer2->value();
  }
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
	default:
	  break;
      }

    }
  }

  uint32_t errorEntriesCount = read_s4be();
  cout << "\nerrorEntriesCount = " << errorEntriesCount;

  uint32_t channelsLength = read_s4be();
  cout << "\nchannelsLength = " << channelsLength;

}

void string_cp_info_t::_read() 
{
    cout << "\n\nReading String\n\n";
    m_str_len = read_s4be();
    cout << "\nReading string pool, STRLEN = " << m_str_len;
    std::vector<char> result(m_str_len);
    is.read(&result[0], m_str_len);
    m_value = std::string(result.begin(), result.end());
    cout << "\nm_value = " << m_value;
}

void shape_cp_info_t::_read() 
{
    cout << "\n\nReading Shape\n\n";
    m_shape_lenght = read_s4be();
    cout << "\n\nShape length " << m_shape_lenght;
    cout << "\nReading shape pool and shape length, STRLEN = " << m_shape_lenght;
    std::vector<char> result(m_shape_lenght);
    is.read(&result[0], m_shape_lenght);
    cout << "\nData = " << std::string(result.begin(), result.end());
}

void package_cp_info_t::_read() 
{
    cout << "\n\nReading Package\n\n";
    m_org_index = read_s4be();
    cout << "m_org_index = " << m_org_index;
    m_name_index = read_s4be();
    cout << "m_name_index = " << m_name_index;
    m_version_index = read_s4be();
    cout << "m_version_index " << m_version_index;
}

void /*constant_pool_entry_t**/ constant_pool_entry_t::_read() 
{
    m_tag = static_cast<constant_pool_entry_t::tag_enum_t>(read_u1());
    n_cp_info = true;
    switch (tag()) {
        case TAG_ENUM_CP_ENTRY_PACKAGE: {
            n_cp_info = false;
    	    package_cp_info_t *m_cp_info = static_cast<package_cp_info_t *> (this);
     	    m_cp_info->_read();
            break;
        }
        case TAG_ENUM_CP_ENTRY_SHAPE: {
            n_cp_info = false;
            shape_cp_info_t *m_cp_info = static_cast<shape_cp_info_t *> (this);
            m_cp_info->_read();
            break;
        }
        case TAG_ENUM_CP_ENTRY_STRING: {
            n_cp_info = false;
            string_cp_info_t *m_cp_info = static_cast<string_cp_info_t *> (this);
            m_cp_info->_read();
            break;
        }
	default:
	  break;
    }
}

void constant_pool_set_t::_read() 
{
    m_constant_pool_count = read_s4be();
    int l_constant_pool_entries = constant_pool_count();
    m_constant_pool_entries = new std::vector<constant_pool_entry_t*>();
    m_constant_pool_entries->reserve(l_constant_pool_entries);
    for (int i = 0; i < l_constant_pool_entries; i++) {
        constant_pool_entry_t::tag_enum_t m_tag = static_cast<constant_pool_entry_t::tag_enum_t>(peek_read_u1());
    switch (m_tag) {
        case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_PACKAGE: {
            package_cp_info_t *m_cp_info = new package_cp_info_t();
            constant_pool_entry_t *pointer = static_cast<constant_pool_entry_t *> (m_cp_info);
            pointer->_read();
            m_constant_pool_entries->push_back(pointer);
            break;
        }
        case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_SHAPE: {
            shape_cp_info_t *m_cp_info = new shape_cp_info_t();
            constant_pool_entry_t *pointer = static_cast<constant_pool_entry_t *> (m_cp_info);
            pointer->_read();
            m_constant_pool_entries->push_back(pointer);
            break;
        }
        case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_STRING: {
            string_cp_info_t *m_cp_info = new string_cp_info_t();
            constant_pool_entry_t *pointer = static_cast<constant_pool_entry_t *> (m_cp_info);
            pointer->_read();
            m_constant_pool_entries->push_back(pointer);
            break;
        }
	default:
	  break;
    }
        cout << "\nRead Constant Pool entry = " << i << "\nNow next entry";
    }
}

void BIRReader::deserialize()
{
    is.open("main-bir-dump", ifstream::binary);

    constant_pool_set_t *m_constant_pool = new constant_pool_set_t();

    // Read module
    class module_t readBIRPackage;// = new module_t();
    //class BIRPackage *BIRpackage = new BIRPackage ();
    readBIRPackage.m_id_cp_index = read_s4be();
    constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(readBIRPackage.m_id_cp_index);
    std::string org;
    std::string name;
    std::string version;
        switch (entry_pointer->tag()) {
       	    case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_PACKAGE: {
	    package_cp_info_t *pointer = static_cast<package_cp_info_t *> (entry_pointer);
	    cout << "\nPointer->m_org_index" << pointer->org_index();
	    constant_pool_entry_t *org_pointer = m_constant_pool->constant_pool_entries()->at(pointer->org_index());
	    string_cp_info_t *org_pointer2 = static_cast<string_cp_info_t *> (org_pointer);
	    cout << "\nOrg name = " << org_pointer2->value();
	    org = org_pointer2->value();
	    cout << "\nPointer->m_org_index" << pointer->name_index();

            constant_pool_entry_t *name_pointer = m_constant_pool->constant_pool_entries()->at(pointer->name_index());
            string_cp_info_t *name_pointer2 = static_cast<string_cp_info_t *> (name_pointer);
            cout << "\nName name = " << name_pointer2->value();
            name = name_pointer2->value();

	    cout << "\nPointer->m_org_index" << pointer->version_index();
            constant_pool_entry_t *version_pointer = m_constant_pool->constant_pool_entries()->at(pointer->version_index());
            string_cp_info_t *version_pointer2 = static_cast<string_cp_info_t *> (version_pointer);
            cout << "\nVersion name = " << version_pointer2->value();
            version = version_pointer2->value();
	    break;
	  }
	  case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_SHAPE: {
	    shape_cp_info_t *pointer = static_cast<shape_cp_info_t *> (entry_pointer);
	    break;
	  }
	  case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_STRING: {
	    string_cp_info_t *pointer = static_cast<string_cp_info_t *> (entry_pointer);
	    break;
	  }
	  default:
	    break;
	}


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

    for (int i = 0; i < readBIRPackage.m_function_count; i++) {
        cout << "\nReading Function = " << i;
        read_function (i, m_constant_pool);
    }

    readBIRPackage.m_annotations_size = read_s4be();
    cout << "\nannotationLength = " << readBIRPackage.m_annotations_size;

}
/*
void BIRReader::readPackages(ifstream dataInStream) 
{
  int symbolCount;
  dataInStream >> symbolCount;
  for (int i = 0; i < symbolCount; i++) {
      dataInStream >> BinBIRPackage;
  }
}

void BIRReader::readFunctions(istream *dataInStream) 
{
  int symbolCount;
  dataInStream >> symbolCount;
  for (int i = 0; i < symbolCount; i++) {
      dataInStream >> BinBIRFunction;
  }
}
*/
