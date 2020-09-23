#include "BIR.h"

BasicBlockT::BasicBlockT() {}

BasicBlockT::BasicBlockT(string pid):id(pid) {
}

BasicBlockT::BasicBlockT(Location *loc, string pid):BIRNode(loc), id(pid)
{
}

LLVMValueRef BasicBlockT::getValueRefBasedOnName (string lhsName) {
  map<string, LLVMValueRef>::iterator it;
  it = branchComplist.find(lhsName);

  if(it == branchComplist.end()) {
    return NULL;
  }
  else
    return it->second;
}

BasicBlockT::~BasicBlockT() {}

void BasicBlockT::translate(LLVMModuleRef &modRef)
{
  for(unsigned int i=0; i < instructions.size(); i++)
  {
    NonTerminatorInsn *insn = instructions[i];

    insn->setFunction(BFunc);
    insn->setcurrentBB(this);
    switch (insn->getInstKind()) {
      case INSTRUCTION_KIND_MOVE:
      {
        MoveInsn *moveInsn = static_cast<MoveInsn*>(insn);
        moveInsn->translate(modRef);
        break;
      }
      case INSTRUCTION_KIND_BINARY_ADD:
      case INSTRUCTION_KIND_BINARY_SUB:
      case INSTRUCTION_KIND_BINARY_MUL:
      case INSTRUCTION_KIND_BINARY_DIV:
      case INSTRUCTION_KIND_BINARY_EQUAL:
      case INSTRUCTION_KIND_BINARY_NOT_EQUAL:
      case INSTRUCTION_KIND_BINARY_MOD:
      case INSTRUCTION_KIND_BINARY_GREATER_THAN:
      case INSTRUCTION_KIND_BINARY_GREATER_EQUAL:
      case INSTRUCTION_KIND_BINARY_LESS_THAN:
      case INSTRUCTION_KIND_BINARY_LESS_EQUAL:
      case INSTRUCTION_KIND_BINARY_BITWISE_XOR:
      {
        BinaryOpInsn *binOpInsn = static_cast<BinaryOpInsn*>(insn);
        binOpInsn->translate(modRef);
        break;
      }

      case INSTRUCTION_KIND_CONST_LOAD:
      {
        ConstantLoadInsn *constL = static_cast<ConstantLoadInsn*>(insn);
        constL->translate(modRef);
        break;
      }
      case INSTRUCTION_KIND_UNARY_NEG:
      case INSTRUCTION_KIND_UNARY_NOT:
      {
	UnaryOpInsn *unaryOpIn = static_cast<UnaryOpInsn*>(insn);
	unaryOpIn->translate(modRef);
	break;
      }
      default:
        break;
    } 
  }

  if(terminator) 
  {
    terminator->setFunction(BFunc);
    terminator->setcurrentBB(this);
    switch (terminator->getInstKind()) {
      case INSTRUCTION_KIND_CONDITIONAL_BRANCH:
      {
	ConditionBrInsn *condBr = static_cast<ConditionBrInsn*>(terminator);
	condBr->translate(modRef);
	break;
      }
      case INSTRUCTION_KIND_GOTO:
      {
        GoToInsn *gotoInsn = static_cast<GoToInsn*>(terminator);
        gotoInsn->translat(modRef);
        break;
      }
      case INSTRUCTION_KIND_RETURN:
      {
        ReturnInsn *returnInsn = static_cast<ReturnInsn*>(terminator);
        returnInsn->translat(modRef);
        break;
      }
      default:
        break;
    }
  }
}
