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

// Search string from the constant pool based on index
int32_t get_int_cp (int32_t index, constant_pool_set_t *m_constant_pool)
{
    constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(index);
    int_cp_info_t *int_cp = static_cast<int_cp_info_t *> (entry_pointer);
    return int_cp->value();
}

// Search type from the constant pool based on index
TypeDecl* get_type_cp(int32_t index, constant_pool_set_t *m_constant_pool, bool voidToInt)
{
    constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(index);
    shape_cp_info_t *shape_cp = static_cast<shape_cp_info_t *> (entry_pointer);
    TypeDecl *typeDecl = new TypeDecl();
    typeDecl->setTypeDeclName(get_string_cp(shape_cp->name_index(), m_constant_pool));
    if (typeDecl->getTypeDeclName() == "")
    {
      char  newName[20];
      char *p;
      p = stpcpy(newName, "anon-");
      sprintf(p,  "%5ld", random() % 100000);
      typeDecl->setTypeDeclName(newName);
    }
    typeDecl->setTypeTag(shape_cp->type_tag());
    if (shape_cp->type_tag() == TYPE_TAG_ENUM_TYPE_TAG_NIL && voidToInt)
      typeDecl->setTypeTag(TYPE_TAG_ENUM_TYPE_TAG_INT);
    typeDecl->setFlags(shape_cp->type_flag());
    return typeDecl;
}

// Get the Type tag from the constant pool based on the index passed
type_tag_enum_t get_type_tag(int32_t index, constant_pool_set_t *m_constant_pool)
{
    constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(index);
    shape_cp_info_t *shape_cp = static_cast<shape_cp_info_t *> (entry_pointer);
    return shape_cp->type_tag();
}

// Read Global Variable and push it to BIRPackage
void read_global_var(constant_pool_set_t *m_constant_pool, BIRPackage *BIRpackage)
{
    VarDecl *varDecl = new VarDecl();
    uint8_t kind = read_u1();
    varDecl->setVarKind((VarKind)kind);

    uint32_t varDclNameCpIndex = read_s4be();
    varDecl->setVarName((get_string_cp(varDclNameCpIndex, m_constant_pool)));

    uint32_t flags __attribute__((unused)) = read_s4be();
    uint32_t length __attribute__((unused)) = read_s4be();
    uint8_t hasDoc = read_u1();
    if (hasDoc)
    {
        uint32_t description_cp_index __attribute__((unused)) = read_s4be();
        uint32_t return_value_description_cp_index __attribute__((unused)) = read_s4be();
        uint32_t parameters_count __attribute__((unused)) = read_s4be();
    }
    uint32_t typeCpIndex = read_s4be();
    TypeDecl *typeDecl = get_type_cp(typeCpIndex, m_constant_pool, false);
    varDecl->setTypeDecl(typeDecl);
    BIRpackage->addGlobalVar(varDecl);
}

// Read Local Variable and return VarDecl pointer
VarDecl* read_variable (constant_pool_set_t *m_constant_pool)
{
    VarDecl *varDecl = new VarDecl();
    uint8_t ignoredVar = read_u1();
    varDecl->setIgnore((bool)ignoredVar);

    uint8_t kind = read_u1();
    varDecl->setVarKind((VarKind)kind);

    uint8_t scope = read_u1();
    varDecl->setVarScope((VarScope)scope);

    uint32_t varDclNameCpIndex = read_s4be();
    varDecl->setVarName((get_string_cp(varDclNameCpIndex, m_constant_pool)));

    if (varDecl->getVarKind() == GLOBAL_VAR_KIND)
    {
            uint32_t m_package_index __attribute__((unused)) = read_s4be();
      uint32_t m_type_cp_index __attribute__((unused)) = read_s4be();
    }
    return varDecl;
}

// Read TYPEDESC Insn
void read_typedesc ()
{
    uint8_t ignoredVar __attribute__((unused));
    ignoredVar = read_u1();

    uint8_t kind __attribute__((unused));
    kind = read_u1();

    uint8_t scope __attribute__((unused));
    scope = read_u1();

    uint32_t varDclNameCpIndex __attribute__((unused));
    varDclNameCpIndex = read_s4be();

    uint32_t typeCpIndex __attribute__((unused));
    typeCpIndex = read_s4be();
}

// Read STRUCTURE Insn
void read_structure ()
{
    uint8_t ignoredVar __attribute__((unused));
    ignoredVar = read_u1();

    uint8_t kind __attribute__((unused));
    kind = read_u1();

    uint8_t scope __attribute__((unused));
    scope = read_u1();

    uint32_t varDclNameCpIndex __attribute__((unused));
    varDclNameCpIndex = read_s4be();

    uint8_t ignoredVar1 __attribute__((unused));
    ignoredVar1 = read_u1();

    uint8_t kind1 __attribute__((unused));
    kind1 = read_u1();

    uint8_t scope1 __attribute__((unused));
    scope1 = read_u1();

    uint32_t varDclNameCpIndex1 __attribute__((unused));
    varDclNameCpIndex1 = read_s4be();

    uint32_t packageIndex1 __attribute__((unused));
    packageIndex1 = read_s4be();

    uint32_t typeCpIndex1 __attribute__((unused));
    typeCpIndex1 = read_s4be();
}

// Read CONST_LOAD Insn
void read_const (ConstantLoadInsn *constantloadInsn, constant_pool_set_t *m_constant_pool)
{
    Operand *lhsOperand = new Operand();
    VarDecl *varDecl = new VarDecl();
    lhsOperand->setVarDecl(varDecl);

    uint32_t typeCpIndex1 = read_s4be();
    TypeDecl *typeDecl = get_type_cp(typeCpIndex1, m_constant_pool, false);
    varDecl->setTypeDecl(typeDecl);

    uint8_t ignoredVar = read_u1();
    lhsOperand->getVarDecl()->setIgnore((bool)ignoredVar);

    uint8_t kind = read_u1();
    lhsOperand->getVarDecl()->setVarKind((VarKind)kind);

    uint8_t scope = read_u1();
    lhsOperand->getVarDecl()->setVarScope((VarScope)scope);

    uint32_t varDclNameCpIndex = read_s4be();
    lhsOperand->getVarDecl()->setVarName((get_string_cp(varDclNameCpIndex, m_constant_pool)));

    if (lhsOperand->getVarDecl()->getVarKind() == GLOBAL_VAR_KIND)
    {
        uint32_t packageIndex __attribute__((unused)) = read_s4be();
        uint32_t typeCpIndex __attribute__((unused)) = read_s4be();
    }

    type_tag_enum_t m_type_tag = get_type_tag(typeCpIndex1, m_constant_pool);
    if (m_type_tag == TYPE_TAG_ENUM_TYPE_TAG_INT)
    {
      uint32_t valueCpIndex = read_s4be();
      constantloadInsn->setValue(get_int_cp(valueCpIndex, m_constant_pool));
    }
    if (m_type_tag == TYPE_TAG_ENUM_TYPE_TAG_BOOLEAN)
    {
      uint8_t valueCpIndex = read_u1();
      constantloadInsn->setValue(valueCpIndex);
    }

    constantloadInsn->setLhsOperand(lhsOperand);
}

// Read GOTO Insn
uint32_t read_goto ()
{
    uint32_t targetBBNameCpIndex = read_s4be();
    return targetBBNameCpIndex;
}

// Read Unary Operand
UnaryOpInsn* read_unaryOp(UnaryOpInsn *unaryOpInsn, constant_pool_set_t *m_constant_pool)
{
    VarDecl *varDecl1 = read_variable (m_constant_pool);
    VarDecl *varDecl2 = read_variable (m_constant_pool);

    Operand  *rhsOp = new Operand();
    rhsOp->setVarDecl(varDecl1);

    Operand  *lhsOp = new Operand();
    lhsOp->setVarDecl(varDecl2);

    unaryOpInsn->setRhsOp(rhsOp);
    unaryOpInsn->setLhsOperand(lhsOp);
    return unaryOpInsn;
}

// Read Binary Operand
BinaryOpInsn* read_binaryOp(BinaryOpInsn *binaryOpInsn, constant_pool_set_t *m_constant_pool)
{
    VarDecl *varDecl1 = read_variable (m_constant_pool);
    VarDecl *varDecl2 = read_variable (m_constant_pool);
    VarDecl *varDecl3 = read_variable (m_constant_pool);

    Operand  *rhsOp1 = new Operand();
    rhsOp1->setVarDecl(varDecl1);

    Operand  *rhsOp2 = new Operand();
    rhsOp2->setVarDecl(varDecl2);

    Operand  *lhsOp = new Operand();
    lhsOp->setVarDecl(varDecl3);

    binaryOpInsn->setRhsOp1(rhsOp1);
    binaryOpInsn->setRhsOp2(rhsOp2);
    binaryOpInsn->setLhsOperand(lhsOp);
    return binaryOpInsn;
}

// Read BRANCH Insn
ConditionBrInsn* read_conditionalBr(ConditionBrInsn *conditionBrInsn, constant_pool_set_t *m_constant_pool)
{
    VarDecl *varDecl = read_variable (m_constant_pool);
    Operand  *lhsOp = new Operand();
    lhsOp->setVarDecl(varDecl);
    conditionBrInsn->setLhsOperand(lhsOp);
    uint32_t trueBbIdNameCpIndex = read_s4be();
    uint32_t falseBbIdNameCpIndex = read_s4be();

    BIRBasicBlock *trueDummybasicBlock = new BIRBasicBlock(get_string_cp(trueBbIdNameCpIndex, m_constant_pool));
    conditionBrInsn->setIfThenBB(trueDummybasicBlock);

    BIRBasicBlock *falseDummybasicBlock = new BIRBasicBlock(get_string_cp(falseBbIdNameCpIndex, m_constant_pool));
    conditionBrInsn->setElseBB(falseDummybasicBlock);

    conditionBrInsn->setPatchStatus(true);
    conditionBrInsn->setNextBB(NULL);
    return conditionBrInsn;
}

// Read MOV Insn
MoveInsn* read_move(MoveInsn *moveInsn, constant_pool_set_t *m_constant_pool)
{
    VarDecl *varDecl1 = read_variable (m_constant_pool);
    VarDecl *varDecl2 = read_variable (m_constant_pool);

    Operand  *rhsOp = new Operand();
    rhsOp->setVarDecl(varDecl1);

    Operand  *lhsOp = new Operand();
    lhsOp->setVarDecl(varDecl2);

    moveInsn->setRhsOp(rhsOp);
    moveInsn->setLhsOperand(lhsOp);
    return moveInsn;
}


// Search basic block based on the basic block ID
BIRBasicBlock* search_bb_by_name(vector<BIRBasicBlock *>   basicBlocks, std::string name)
{
    std::vector<BIRBasicBlock *>::iterator itr;
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
NonTerminatorInsn* readInsn (BIRFunction *BIRfunction, BIRBasicBlock *basicBlock, NonTerminatorInsn *nonTerminatorInsn, constant_pool_set_t *m_constant_pool)
{
    uint32_t sLine = read_s4be();
    uint32_t eLine = read_s4be();
    uint32_t sCol = read_s4be();
    uint32_t eCol = read_s4be();
    uint32_t sourceFileCpIndex = read_s4be();

    Location *location = new Location(get_string_cp(sourceFileCpIndex, m_constant_pool), (int)sLine, (int)eLine, (int)sCol, (int)eCol);
    nonTerminatorInsn->setLocation(location);

    uint8_t insnkind = read_u1();
    switch ((InstructionKind)insnkind) {
      case INSTRUCTION_KIND_NEW_TYPEDESC: {
          read_typedesc();
          break;
      }
      case INSTRUCTION_KIND_NEW_STRUCTURE: {
          read_structure();
          break;
      }
      case INSTRUCTION_KIND_CONST_LOAD: {
          ConstantLoadInsn *constantloadInsn = new ConstantLoadInsn();
          constantloadInsn->setFunction(BIRfunction);
          constantloadInsn->setInstKind((InstructionKind)insnkind);

          read_const(constantloadInsn, m_constant_pool);
          nonTerminatorInsn = (static_cast<NonTerminatorInsn *> (constantloadInsn));
          break;
      }
      case INSTRUCTION_KIND_GOTO: {
	  GoToInsn *gotoInsn = new GoToInsn();
          uint32_t nameid = read_goto();
          BIRBasicBlock *dummybasicBlock = new BIRBasicBlock(get_string_cp(nameid, m_constant_pool));
          gotoInsn->setNextBB(dummybasicBlock);
          gotoInsn->setPatchStatus(true);
          gotoInsn->setInstKind((InstructionKind)insnkind);
          basicBlock->setTerminatorInsn(static_cast<TerminatorInsn *> (gotoInsn));
          nonTerminatorInsn = NULL;
          break;
      }
      case INSTRUCTION_KIND_RETURN: {
	  ReturnInsn *returnInsn = new ReturnInsn();
          returnInsn->setInstKind((InstructionKind)insnkind);
          basicBlock->setTerminatorInsn(static_cast<TerminatorInsn *> (returnInsn));
          nonTerminatorInsn = NULL;
          break;
      }
      case INSTRUCTION_KIND_BINARY_ADD:
      case INSTRUCTION_KIND_BINARY_SUB:
      case INSTRUCTION_KIND_BINARY_MUL:
      case INSTRUCTION_KIND_BINARY_DIV:
      case INSTRUCTION_KIND_BINARY_EQUAL:
      case INSTRUCTION_KIND_BINARY_NOT_EQUAL:
      case INSTRUCTION_KIND_BINARY_GREATER_THAN:
      case INSTRUCTION_KIND_BINARY_GREATER_EQUAL:
      case INSTRUCTION_KIND_BINARY_LESS_THAN:
      case INSTRUCTION_KIND_BINARY_LESS_EQUAL:
      case INSTRUCTION_KIND_BINARY_BITWISE_XOR:
      case INSTRUCTION_KIND_BINARY_MOD: {
          BinaryOpInsn *binaryOpInsn = new BinaryOpInsn();
          binaryOpInsn->setInstKind((InstructionKind)insnkind);
          binaryOpInsn = read_binaryOp(binaryOpInsn, m_constant_pool);
          nonTerminatorInsn = (static_cast<NonTerminatorInsn *> (binaryOpInsn));
          break;
      }
      case INSTRUCTION_KIND_UNARY_NEG:
      case INSTRUCTION_KIND_UNARY_NOT: {
          UnaryOpInsn *unaryOpInsn = new UnaryOpInsn();
          unaryOpInsn->setInstKind((InstructionKind)insnkind);
          unaryOpInsn = read_unaryOp(unaryOpInsn, m_constant_pool);
          nonTerminatorInsn = (static_cast<NonTerminatorInsn *> (unaryOpInsn));
          break;
      }
      case INSTRUCTION_KIND_CONDITIONAL_BRANCH: {
          ConditionBrInsn *conditionBrInsn = new ConditionBrInsn();
          conditionBrInsn->setInstKind((InstructionKind)insnkind);
          conditionBrInsn = read_conditionalBr(conditionBrInsn, m_constant_pool);
          basicBlock->setTerminatorInsn(static_cast<TerminatorInsn *> (conditionBrInsn));
          nonTerminatorInsn = NULL;
          break;
      }
      case INSTRUCTION_KIND_MOVE: {
          MoveInsn *moveInsn = new MoveInsn();
          moveInsn->setInstKind((InstructionKind)insnkind);
          moveInsn = read_move(moveInsn, m_constant_pool);
          nonTerminatorInsn = (static_cast<NonTerminatorInsn *> (moveInsn));
          break;
      }
      default:
          break;
    }
    return nonTerminatorInsn;
}

// Read Basic Block from the BIR
BIRBasicBlock* readBasicBlock (BIRFunction *BIRfunction, BIRBasicBlock *basicBlock, constant_pool_set_t *m_constant_pool)
{
    uint32_t nameCpIndex = read_s4be();
    basicBlock->setId(get_string_cp(nameCpIndex, m_constant_pool));
    basicBlock->setBIRFunction(BIRfunction);

    uint32_t insnCount = read_s4be();
    for (unsigned int i = 0; i < insnCount; i++)
    {
      NonTerminatorInsn *nonTerminatorInsn = new NonTerminatorInsn();
      nonTerminatorInsn->setFunction(BIRfunction);
      nonTerminatorInsn = readInsn(BIRfunction, basicBlock, nonTerminatorInsn, m_constant_pool);
      if (nonTerminatorInsn)
        basicBlock->addNonTermInsn(nonTerminatorInsn);
    }
  return basicBlock;
}

// Patches the Terminator Insn with destination Basic Block
void patchInsn(vector<BIRBasicBlock *>   basicBlocks)
{
    for (unsigned int i = 0; i < basicBlocks.size(); i++)
    {
        BIRBasicBlock *basicBlock = basicBlocks[i];
        TerminatorInsn *terminator = basicBlock->getTerminatorInsn();
        if (terminator->getPatchStatus())
        {
          switch (terminator->getInstKind()) {
            case INSTRUCTION_KIND_CONDITIONAL_BRANCH :
            {
                ConditionBrInsn *Terminator = (static_cast<ConditionBrInsn *> (terminator));
                BIRBasicBlock *trueBB = search_bb_by_name(basicBlocks, Terminator->getIfThenBB()->getId());
                BIRBasicBlock *falseBB = search_bb_by_name(basicBlocks, Terminator->getElseBB()->getId());
                BIRBasicBlock *danglingTrueBB = Terminator->getIfThenBB();
                BIRBasicBlock *danglingFalseBB = Terminator->getElseBB();
                delete danglingTrueBB;
                delete danglingFalseBB;
                Terminator->setIfThenBB(trueBB);
                Terminator->setElseBB(falseBB);
                break;
            }
            case INSTRUCTION_KIND_GOTO :
            {
                BIRBasicBlock *destBB = search_bb_by_name(basicBlocks, terminator->getNextBB()->getId());
                BIRBasicBlock *danglingBB = terminator->getNextBB();
                delete danglingBB;
                terminator->setNextBB(destBB);
                break;
            }
            default:
                fprintf(stderr, "%s:%d Invalid Insn Kind for Instruction Patching.\n", __FILE__, __LINE__);
                break;
           }
        }
    }
}

// Reads BIR function 
BIRFunction* read_function (constant_pool_set_t *m_constant_pool, BIRPackage *BIRpackage)
{
    BIRFunction *BIRfunction = new BIRFunction();
    uint32_t sLine = read_s4be();
    uint32_t eLine = read_s4be();
    uint32_t sCol = read_s4be();
    uint32_t eCol = read_s4be();

    uint32_t sourceFileCpIndex = read_s4be();
    BIRpackage->setSrcFileName(get_string_cp(sourceFileCpIndex, m_constant_pool));

    Location *location = new Location(get_string_cp(sourceFileCpIndex, m_constant_pool), (int)sLine, (int)eLine, (int)sCol, (int)eCol);
    BIRfunction->setLocation(location);

    uint32_t nameCpIndex = read_s4be();
    BIRfunction->setName(get_string_cp(nameCpIndex, m_constant_pool));

    uint32_t workdernameCpIndex = read_s4be();
    BIRfunction->setWorkerName(get_string_cp(workdernameCpIndex, m_constant_pool));

    uint32_t flags = read_s4be();
    BIRfunction->setFlags(flags);

    uint32_t typeCpIndex __attribute__((unused));
    typeCpIndex = read_s4be();

    uint64_t annotation_length __attribute__((unused));
    annotation_length = read_s8be();

    uint32_t annotationAttachments __attribute__((unused));
    annotationAttachments = read_s4be();

    uint32_t requiredParamCount = read_s4be();
    BIRfunction->setNumParams(requiredParamCount);

    uint8_t hasRestParam = read_u1();
    if (!((bool)hasRestParam))
      BIRfunction->setRestParam(NULL);

    uint8_t hasReceiver = read_u1();
    if (!((bool)hasReceiver))
      BIRfunction->setReceiver(NULL);

    uint64_t taintTableLength = read_s8be();

    std::vector<char> result(taintTableLength);
    is.read(&result[0], taintTableLength);

    uint32_t docLength __attribute__((unused));
    docLength = read_s4be();

    std::vector<char> result1(docLength);
    is.read(&result1[0], docLength);

    uint64_t functionBodyLength __attribute__((unused));
    functionBodyLength = read_s8be();

    uint32_t argsCount __attribute__((unused));
    argsCount = read_s4be();

    uint8_t m_has_return_var = read_u1();

    if (m_has_return_var)
    {
        VarDecl *varDecl = new VarDecl();

        uint8_t kind = read_u1();
        varDecl->setVarKind ((VarKind)kind);

        uint32_t typeCpIndex = read_s4be();
        TypeDecl *typeDecl = get_type_cp(typeCpIndex, m_constant_pool, false);
        varDecl->setTypeDecl(typeDecl);

        uint32_t nameCpIndex = read_s4be();
        varDecl->setVarName(get_string_cp(nameCpIndex, m_constant_pool));

        BIRfunction->setReturnVar(varDecl);
    }

    uint32_t defaultParamValue __attribute__((unused));
    defaultParamValue = read_s4be();

    uint32_t localVarCount = read_s4be();

    std::vector<VarDecl *> localvars;
    for (unsigned int i = 0; i < localVarCount; i++)
    {
        VarDecl *varDecl = new VarDecl();

        uint8_t kind = read_u1();
        varDecl->setVarKind ((VarKind)kind);

        uint32_t typeCpIndex = read_s4be();
        TypeDecl *typeDecl = get_type_cp(typeCpIndex, m_constant_pool, false);
        varDecl->setTypeDecl(typeDecl);

        uint32_t nameCpIndex = read_s4be();
        varDecl->setVarName(get_string_cp(nameCpIndex, m_constant_pool));
        localvars.push_back(varDecl);

        if (kind == 1)
        {
	    uint32_t metaVarNameCpIndex __attribute__((unused)) = read_s4be();
	    uint32_t endBbIdCpIndex __attribute__((unused)) = read_s4be();
	    uint32_t startBbIdCpIndex __attribute__((unused)) = read_s4be();
	    uint32_t instructionOffset __attribute__((unused)) = read_s4be();
        }

    }
    BIRfunction->setLocalVars(localvars);

    uint32_t BBCount = read_s4be();

    for (unsigned int i = 0; i < BBCount; i++)
    {
      BIRBasicBlock *basicBlock = new BIRBasicBlock();
      basicBlock = readBasicBlock(BIRfunction, basicBlock, m_constant_pool);
      BIRfunction->addBIRBasicBlock(basicBlock);
    }

    //Patching the insn
    vector<BIRBasicBlock *>   basicBlocks = BIRfunction->getBasicBlocks();
    patchInsn(basicBlocks);

    //Create links between Basic Block
    for (unsigned int i = 0; i < BBCount - 1; i++)
    {
      BIRBasicBlock *basicBlock = BIRfunction->getBasicBlock(i);
      basicBlock->setNextBB(BIRfunction->getBasicBlock(i + 1));
    }

    uint32_t errorEntriesCount __attribute__((unused));
    errorEntriesCount = read_s4be();

    uint32_t channelsLength __attribute__((unused));
    channelsLength = read_s4be();

    return BIRfunction;
}

void string_cp_info_t::_read() 
{
    m_str_len = read_s4be();
    std::vector<char> result(m_str_len);
    is.read(&result[0], m_str_len);
    m_value = std::string(result.begin(), result.end());
}

void shape_cp_info_t::_read() 
{
    m_shape_lenght = read_s4be();
    std::vector<char> result(m_shape_lenght);
    is.read(&result[0], m_shape_lenght);

    m_type_tag = static_cast<type_tag_enum_t>(result[0]);
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

    m_name_index = temp_name_index;
    m_type_flag = temp_type_flag;
}

void package_cp_info_t::_read() 
{
    m_org_index = read_s4be();
    m_name_index = read_s4be();
    m_version_index = read_s4be();
}

void int_cp_info_t::_read() {
    m_value = read_s8be();
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
        case TAG_ENUM_CP_ENTRY_INTEGER: {
            n_cp_info = false;
            int_cp_info_t *m_cp_info = static_cast<int_cp_info_t *> (this);
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
        case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_INTEGER: {
            int_cp_info_t *m_cp_info = new int_cp_info_t();
            constant_pool_entry_t *pointer = static_cast<constant_pool_entry_t *> (m_cp_info);
            pointer->_read();
            m_constant_pool_entries->push_back(pointer);
            break;
        }
        default:
            break;
       }
    }
}

void BIRReader::deserialize(BIRPackage *BIRpackage)
{
    is.open(filename, ifstream::binary);

    constant_pool_set_t *m_constant_pool = new constant_pool_set_t();
    m_constant_pool->_read();

    // Read module
    int32_t m_id_cp_index = read_s4be();
    constant_pool_entry_t *entry_pointer = m_constant_pool->constant_pool_entries()->at(m_id_cp_index);
        switch (entry_pointer->tag()) {
           case constant_pool_entry_t::tag_enum_t::TAG_ENUM_CP_ENTRY_PACKAGE: {
                 package_cp_info_t *pointer = static_cast<package_cp_info_t *> (entry_pointer);
                 constant_pool_entry_t *org_pointer = m_constant_pool->constant_pool_entries()->at(pointer->org_index());
                 string_cp_info_t *org_pointer2 = static_cast<string_cp_info_t *> (org_pointer);
                 BIRpackage->setOrgName(org_pointer2->value());

                 constant_pool_entry_t *name_pointer = m_constant_pool->constant_pool_entries()->at(pointer->name_index());
                 string_cp_info_t *name_pointer2 = static_cast<string_cp_info_t *> (name_pointer);
                 BIRpackage->setPackageName(name_pointer2->value());

                 constant_pool_entry_t *version_pointer = m_constant_pool->constant_pool_entries()->at(pointer->version_index());
                 string_cp_info_t *version_pointer2 = static_cast<string_cp_info_t *> (version_pointer);
                 BIRpackage->setVersion(version_pointer2->value());
                 break;
           }
           default:
             break;
        }


    // The following three are read into unused variables so that the file
    // pointer advances to the data that we need next.
    uint32_t m_import_count __attribute__((unused));
    uint32_t m_const_count __attribute__((unused));
    uint32_t m_type_definition_count __attribute__((unused));

    m_import_count = read_s4be();
    m_const_count = read_s4be();
    m_type_definition_count = read_s4be();

    uint32_t m_golbal_var_count = read_s4be();
    if (m_golbal_var_count > 0)
    {
	for (unsigned int i = 0; i < m_golbal_var_count; i++)
	{
	    read_global_var(m_constant_pool, BIRpackage);
	}
    }

    uint32_t m_type_definition_bodies_count __attribute__((unused));
    m_type_definition_bodies_count = read_s4be();
    uint32_t m_function_count = read_s4be();

    std::string funcName = "main";
    for (unsigned int i = 0; i < m_function_count; i++) {
        BIRFunction *curFunc = read_function (m_constant_pool, BIRpackage);
        if (funcName.compare (curFunc->getName()) == 0)
           BIRpackage->addFunction(curFunc);
        else
           delete curFunc;
    }

    uint32_t m_annotations_size __attribute__((unused));
    m_annotations_size = read_s4be();
}
