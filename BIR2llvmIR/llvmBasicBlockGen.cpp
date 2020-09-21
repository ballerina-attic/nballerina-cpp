#include "BIR.h"

BasicBlockT::BasicBlockT() {}

BasicBlockT::BasicBlockT(string pid):id(pid) {
}

BasicBlockT::BasicBlockT(Location *loc, string pid):BIRNode(loc), id(pid)
{
}
BasicBlockT::~BasicBlockT() {}

void BasicBlockT::translate(LLVMModuleRef &modRef)
{
  for(unsigned int i=0; i < instructions.size(); i++)
  {
    NonTerminatorInsn *insn = instructions[i];

    insn->setFunction(BFunc);
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
    switch (terminator->getInstKind()) {
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
