#include "BIR.h"
#include "BIRReader.h"
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
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

// Search string from the constant pool based on index
std::string get_string_cp (int32_t index, constant_pool_set_t *m_constant_pool)
{
	constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(index);
        string_cp_info_t *string_cp = static_cast<string_cp_info_t *> (entry_pointer);
	return string_cp->value();
}

// Search type from the constant pool based on index
TypeDecl* get_type_cp(int32_t index, constant_pool_set_t *m_constant_pool)
{
        constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(index);
        shape_cp_info_t *shape_cp = static_cast<shape_cp_info_t *> (entry_pointer);
        class TypeDecl *typeDecl = new TypeDecl();
        typeDecl->setTypeDeclName(get_string_cp(shape_cp->name_index(), m_constant_pool));
	if (typeDecl->getTypeDeclName() == "")
        {
	  char  newName[20];
	  char *p;
	  p = stpcpy(newName, "anon-");
	  sprintf(p,  "%5d", random() % 100000);
	  typeDecl->setTypeDeclName(newName);
	  cout << "\n\nUsed tempnam for TypeDecl with value newName = " << newName << endl;
	  /*char temp[] = "$anon";
          std::string altName = mkstemp(temp);
	  typeDecl->setTypeDeclName(altName);
	  cout << "\n\nUsed tempnam for TypeDecl with value altName = " << altName << endl;*/
        }
        typeDecl->setTypeTag(shape_cp->type_tag());
	if (shape_cp->type_tag() == TYPE_TAG_ENUM_TYPE_TAG_NIL)
	  typeDecl->setTypeTag(TYPE_TAG_ENUM_TYPE_TAG_INT);
        typeDecl->setFlags(shape_cp->type_flag());
    return typeDecl;
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

void read_const (int i, ConstantLoadInsn *constantloadInsn, constant_pool_set_t *m_constant_pool)
{
      class Operand *lhsOperand = new Operand();
      class VarDecl *varDecl = new VarDecl();
      lhsOperand->setVarDecl(varDecl);

      uint32_t typeCpIndex1 = read_s4be();
      cout << "\ntypeCpIndex1 = " << typeCpIndex1;
      TypeDecl *typeDecl = get_type_cp(typeCpIndex1, m_constant_pool);
      varDecl->setTypeDecl(typeDecl);

      uint8_t ignoredVar = read_u1();
      cout << "\nignoredVar = " << ignoredVar;
      lhsOperand->getVarDecl()->setIgnore((bool)ignoredVar);

      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;
      lhsOperand->getVarDecl()->setVarKind((VarKind)kind);

      uint8_t scope = read_u1();
      cout << "\nscope = " << (int)scope;
      lhsOperand->getVarDecl()->setVarScope((VarScope)scope);

      uint32_t varDclNameCpIndex = read_s4be();
      cout << "\nvarDclNameCpIndex = " << varDclNameCpIndex;
      lhsOperand->getVarDecl()->setVarName((get_string_cp(varDclNameCpIndex, m_constant_pool)));

      constantloadInsn->setLhsOperand(lhsOperand);
}

uint32_t read_goto ()
{
    uint32_t targetBBNameCpIndex = read_s4be();
    cout << "\ntargetBBNameCpIndex = " << targetBBNameCpIndex;
    return targetBBNameCpIndex;
}

// Search basic block based on the basic block ID
BasicBlockT* search_bb_by_name(vector<BasicBlockT *>   basicBlocks, std::string name)
{
    std::vector<BasicBlockT *>::iterator itr;
    for (itr = basicBlocks.begin(); itr != basicBlocks.end(); itr++)
    {
        if ((*itr)->getId() == name)
        {
            return (*itr);
        }
    }
    return NULL;
}

// Read NonTerminatorInsn from the BIR
NonTerminatorInsn* readInsn (BIRFunction *BIRfunction, int i, BasicBlockT *basicBlock, NonTerminatorInsn *nonTerminatorInsn, constant_pool_set_t *m_constant_pool)
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
      cout << "\ninsnkind = " << (InstructionKind)insnkind;
      switch ((InstructionKind)insnkind) {
        case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_TYPEDESC: {
            read_typedesc();
            break;
        }
        case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_NEW_STRUCTURE: {
            read_structure();
            break;
        }
        case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_CONST_LOAD: {
            class ConstantLoadInsn *constantloadInsn = new ConstantLoadInsn();
            constantloadInsn->setFunction(BIRfunction);
            constantloadInsn->setInstKind((InstructionKind)insnkind);

            read_const(i, constantloadInsn, m_constant_pool);
	    constantloadInsn->setValue(0);
            nonTerminatorInsn = (static_cast<NonTerminatorInsn *> (constantloadInsn));
            break;
        }
        case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_GOTO: {
            class TerminatorInsn *terminatorInsn = new TerminatorInsn();
            uint32_t nameid = read_goto();
	    class BasicBlockT *dummybasicBlock = new BasicBlockT(get_string_cp(nameid, m_constant_pool));
	    terminatorInsn->setNextBB(dummybasicBlock);
	    terminatorInsn->setPatchStatus(true);
            terminatorInsn->setInstKind((InstructionKind)insnkind);
            basicBlock->setTerminatorInsn(terminatorInsn);
	    nonTerminatorInsn = NULL;
            break;
        }
        case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_RETURN: {
            class TerminatorInsn *terminatorInsn = new TerminatorInsn();
            basicBlock->setTerminatorInsn(terminatorInsn);
	    nonTerminatorInsn = NULL;
            //read_return();
            break;
        }
        default:
            break;
      }
      return nonTerminatorInsn;
}

// Read Basic Block from the BIR
BasicBlockT* readBasicBlock (int i, BIRFunction *BIRfunction, BasicBlockT *basicBlock, constant_pool_set_t *m_constant_pool)
{
    cout << "\nReading " << i << "th basic block\n";
    uint32_t nameCpIndex = read_s4be();
    cout << "\nnameCpIndex = " << nameCpIndex;
    basicBlock->setId(get_string_cp(nameCpIndex, m_constant_pool));
    basicBlock->setBIRFunction(BIRfunction);

    uint32_t insnCount = read_s4be();
    cout << "\ninsnCount = " << insnCount;
    cout << "\nReading instructions from BB\n";
    for (unsigned int i = 0; i < insnCount; i++)
    {
      class NonTerminatorInsn *nonTerminatorInsn = new NonTerminatorInsn();
      nonTerminatorInsn->setFunction(BIRfunction);
      nonTerminatorInsn = readInsn(BIRfunction, i, basicBlock, nonTerminatorInsn, m_constant_pool);
      if (nonTerminatorInsn)
        basicBlock->addNonTermInsn(nonTerminatorInsn);
    }
  return basicBlock;
}

// Patches the Terminator Insn with destination Basic Block
void patchInsn(vector<BasicBlockT *>   basicBlocks)
{
  for (unsigned int i = 0; i < basicBlocks.size(); i++)
  {
      BasicBlockT *basicBlock = basicBlocks[i];
      TerminatorInsn *terminator = basicBlock->getTerminatorInsn();
      if (terminator->getPatchStatus())
      {
          BasicBlockT *destBB = search_bb_by_name(basicBlocks, terminator->getNextBB()->getId());
          BasicBlockT *danglingBB = terminator->getNextBB();
          free(danglingBB);
          terminator->setNextBB(destBB);
      }
  }
}

// Reads main function 
BIRFunction* read_function (int i, constant_pool_set_t *m_constant_pool, BIRPackage *BIRpackage)
{
  cout << "\n\nReading from the read_main_function\n\n";
  class BIRFunction *BIRfunction = new BIRFunction();
  uint32_t m_s_line = read_s4be();
  uint32_t eLine = read_s4be();
  uint32_t sCol = read_s4be();
  uint32_t eCol = read_s4be();

  uint32_t sourceFileCpIndex = read_s4be();
  BIRpackage->setSrcFileName(get_string_cp(sourceFileCpIndex, m_constant_pool));
  uint32_t nameCpIndex = read_s4be();

  cout << "\nFunction name = " << get_string_cp(nameCpIndex, m_constant_pool);
  BIRfunction->setName(get_string_cp(nameCpIndex, m_constant_pool));

  uint32_t workdernameCpIndex = read_s4be();
  cout << "\nFunction Worker name = " << get_string_cp(workdernameCpIndex, m_constant_pool);
  BIRfunction->setWorkerName(get_string_cp(workdernameCpIndex, m_constant_pool));

  uint32_t flags = read_s4be();
  BIRfunction->setFlags(flags);

  uint32_t typeCpIndex = read_s4be();
  cout << "\nReading function body\n";
  cout << "\nfunction.m_position.m_s_line = " << m_s_line;
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
  BIRfunction->setNumParams(requiredParamCount);
  cout << "\nrequiredParamCount printing from Function obj = " << BIRfunction->getNumParams();

  uint8_t hasRestParam = read_u1();
  cout << "\nhasRestParam = " << (bool)hasRestParam;
  if (!((bool)hasRestParam))
    BIRfunction->setRestParam(NULL);

  uint8_t hasReceiver = read_u1();
  cout << "\nhasReceiver = " << (bool)hasReceiver;
  if (!((bool)hasReceiver))
    BIRfunction->setReceiver(NULL);

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
      class VarDecl *varDecl = new VarDecl();

      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;
      varDecl->setVarKind ((VarKind)kind);

      uint32_t typeCpIndex = read_s4be();
      cout << "\ntypeCpIndex = " << typeCpIndex;
      TypeDecl *typeDecl = get_type_cp(typeCpIndex, m_constant_pool);
      varDecl->setTypeDecl(typeDecl);

      uint32_t nameCpIndex = read_s4be();
      cout << "\nnameCpIndex = " << nameCpIndex;
      varDecl->setVarName(get_string_cp(nameCpIndex, m_constant_pool));

      BIRfunction->setReturnVar(varDecl);
  }

  uint32_t defaultParamValue = read_s4be();
  cout << "\ndefaultParamValue = " << defaultParamValue;

  uint32_t localVarCount = read_s4be();
  cout << "\nlocalVarCount = " << localVarCount;

  std::vector<VarDecl *> localvars;
  for (unsigned int i = 0; i < localVarCount; i++)
  {
      class VarDecl *varDecl = new VarDecl();

      cout << "\nLocal variable " << i;
      uint8_t kind = read_u1();
      cout << "\nkind = " << (int)kind;
      varDecl->setVarKind ((VarKind)kind);

      uint32_t typeCpIndex = read_s4be();
      cout << "\ntypeCpIndex = " << typeCpIndex;
      TypeDecl *typeDecl = get_type_cp(typeCpIndex, m_constant_pool);
      varDecl->setTypeDecl(typeDecl);

      uint32_t nameCpIndex = read_s4be();
      cout << "\nnameCpIndex = " << nameCpIndex;
      varDecl->setVarName(get_string_cp(nameCpIndex, m_constant_pool));
      localvars.push_back(varDecl);

  }
  BIRfunction->setLocalVars(localvars);

  uint8_t hasDefaultParamBB = read_u1();
  cout << "\nhasDefaultParamBB = " << (int)hasDefaultParamBB;

  uint32_t BBCount = read_s4be();
  cout << "\nBBCount = " << BBCount;

  for (unsigned int i = 0; i < BBCount; i++)
  {
    class BasicBlockT *basicBlock = new BasicBlockT();
    basicBlock = readBasicBlock(i, BIRfunction, basicBlock, m_constant_pool);
    BIRfunction->setBasicBlockT(basicBlock);
  }

  //Patching the insn
  vector<BasicBlockT *>   basicBlocks = BIRfunction->getBasicBlocks();
  patchInsn(basicBlocks);

  //Create links between Basic Block
  for (unsigned int i = 0; i < BBCount - 1; i++)
  {
    class BasicBlockT *basicBlock = BIRfunction->getBasicBlock(i);
    basicBlock->setNextBB(BIRfunction->getBasicBlock(i + 1));
  }

  uint32_t errorEntriesCount = read_s4be();
  cout << "\nerrorEntriesCount = " << errorEntriesCount;

  uint32_t channelsLength = read_s4be();
  cout << "\nchannelsLength = " << channelsLength;

  return BIRfunction;
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

    m_type_tag = static_cast<type_tag_enum_t>(result[0]);
    cout << "\nShape Tag =  " << m_type_tag;

    std::vector<char> result_swap = result;
    std::vector<char>::iterator p = result_swap.begin();

    int temp_name_index = 0;
    int temp_type_flag = 0;
    char tmp;

    //Read name index
    tmp = p[1];
    p[1] = p[4];
    p[4] = tmp;

    tmp = p[2];
    p[2] = p[3];
    p[3] = tmp;

    temp_name_index |= p[4] << 24;
    temp_name_index |= p[3] << 16;
    temp_name_index |= p[2] << 8;
    temp_name_index |= p[1]; 

    //Read type Flag
    tmp = p[5];
    p[5] = p[8];
    p[8] = tmp;

    tmp = p[6];
    p[6] = p[7];
    p[7] = tmp;

    temp_type_flag |= p[8] << 24;
    temp_type_flag |= p[7] << 16;
    temp_type_flag |= p[6] << 8;
    temp_type_flag |= p[5];

    cout << "\nShape name index =  " << temp_name_index;
    m_name_index = temp_name_index;
    cout << "\nname index =  " << m_name_index;

    cout << "\nShape type flag =  " << temp_type_flag;
    m_type_flag = temp_type_flag;
    cout << "\nType Flag =  " << m_type_flag;

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

void constant_pool_entry_t::_read() 
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

void read_BIR_package (BIRPackage *BIRpackage)
{
    cout << "\n\nReading final BIR Package\n\n";
    cout << "\norg = " << BIRpackage->getOrgName();    
    cout << "\nname =" << BIRpackage->getPackageName();    
    cout << "\nversion =" << BIRpackage->getVersion();    
    cout << "\nsrcfilename = " << BIRpackage->getSrcFileName();    
    std::vector<BasicBlockT *>::iterator itr;
    BIRFunction *BIRfunction = BIRpackage->getFunction(0);
    vector<BasicBlockT *>   basicBlocks = BIRfunction->getBasicBlocks();
    
    int i=0,j=0;
    for (itr = basicBlocks.begin(); itr != basicBlocks.end(); itr++)
    {
        cout << i++ << "th basic block\n";
        BasicBlockT *basicblock = (*itr);
        cout << "\nbb name = " << basicblock->getId() << endl;
	vector<NonTerminatorInsn *>  instructions = basicblock->getNonTerminatorInsn();
	std::vector<NonTerminatorInsn *>::iterator itr2;
        for (itr2 = instructions.begin(); itr2 != instructions.end(); itr2++)
        {
            cout << j++ << "th insn\n"; 
        }
        j=0; 
    }
}

BIRPackage* BIRReader::deserialize(BIRPackage *BIRpackage)
{
    //is.open("main-bir-dump", ifstream::binary);
    is.open(filename, ifstream::binary);

    constant_pool_set_t *m_constant_pool = new constant_pool_set_t();

    // Read module
    int32_t m_id_cp_index = read_s4be();
    constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(m_id_cp_index);
        switch (entry_pointer->tag()) {
       	    case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_PACKAGE: {
		 package_cp_info_t *pointer = static_cast<package_cp_info_t *> (entry_pointer);
		 cout << "\nPointer->m_org_index" << pointer->org_index();
		 constant_pool_entry_t *org_pointer = m_constant_pool->constant_pool_entries()->at(pointer->org_index());
		 string_cp_info_t *org_pointer2 = static_cast<string_cp_info_t *> (org_pointer);
		 cout << "\nOrg name = " << org_pointer2->value();
		 BIRpackage->setOrgName(org_pointer2->value());
		 cout << "\nPointer->m_org_index" << pointer->name_index();

		 constant_pool_entry_t *name_pointer = m_constant_pool->constant_pool_entries()->at(pointer->name_index());
		 string_cp_info_t *name_pointer2 = static_cast<string_cp_info_t *> (name_pointer);
		 cout << "\nName name = " << name_pointer2->value();
		 BIRpackage->setPackageName(name_pointer2->value());

 	         cout << "\nPointer->m_org_index" << pointer->version_index();
		 constant_pool_entry_t *version_pointer = m_constant_pool->constant_pool_entries()->at(pointer->version_index());
		 string_cp_info_t *version_pointer2 = static_cast<string_cp_info_t *> (version_pointer);
		 cout << "\nVersion name = " << version_pointer2->value();
		 BIRpackage->setVersion(version_pointer2->value());
		 break;
	   }
	   default:
             break;
	}


    int32_t m_import_count = read_s4be();
    int32_t m_const_count = read_s4be();
    int32_t m_type_definition_count = read_s4be();
    int32_t m_golbal_var_count = read_s4be();
    if (m_golbal_var_count > 0)
    {
        //Read and ignore Global Var Data i.e. 18 bytes
        std::vector<char> result(18);
        is.read(&result[0], 18);
        cout << "\nData = " << std::string(result.begin(), result.end());
    }

    int32_t m_type_definition_bodies_count = read_s4be();
    cout << "\ntypeDefBodyCount = " << m_type_definition_bodies_count;
    int32_t m_function_count = read_s4be();

    cout << "\nFunc count =" << m_function_count;

    std::string funcName = "main";
    for (int i = 0; i < m_function_count; i++) {
        cout << "\nReading Function = " << i;
        BIRFunction *BIRfunction = read_function (i, m_constant_pool, BIRpackage);
	if (funcName.compare (BIRfunction->getName()) == 0)
           BIRpackage->addFunction(BIRfunction);
	else
	   delete(BIRfunction);
    }

    int32_t m_annotations_size = read_s4be();
    cout << "\nannotationLength = " << m_annotations_size;

    read_BIR_package(BIRpackage);
    return BIRpackage;
}
