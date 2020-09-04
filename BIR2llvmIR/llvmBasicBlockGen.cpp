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
  BRef = BFunc->getLLVMBuilder();
  LLVMBasicBlockRef bbRef = LLVMAppendBasicBlock(BFunc->getNewFunctionRef(),
						 (id).c_str());
  LLVMPositionBuilderAtEnd(BRef, bbRef);
  
  for(unsigned int i=0; i < instructions.size(); i++)
  {
    NonTerminatorInsn *insn = instructions[i];
    insn->setFunction(BFunc);
    switch (insn->getInstKind()) {
      case INS_KIND_MOVE:
      {
        MoveInsn *moveInsn = static_cast<MoveInsn*>(insn);
        moveInsn->translate(modRef);
	break;
      }

      case BINARY_ADD:
      case BINARY_SUB:
      case BINARY_MUL:
      case BINARY_DIV:
      case BINARY_EQUAL:
      {
	BinaryOpInsn *binOpInsn = static_cast<BinaryOpInsn*>(insn);
	binOpInsn->translate(modRef);
	break;
      }
      case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_CONST_LOAD:
      {
        ConstantLoadInsn *constL = static_cast<ConstantLoadInsn*>(insn);
        constL->translate(modRef);
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
      case INSTRUCTION_KIND_ENUM_INSTRUCTION_KIND_GOTO:
      {
        GoToInsn *gotoInsn = static_cast<GoToInsn*>(terminator);
        gotoInsn->translate(modRef);
        break;
      }
      default:
	break;
    }
  }
}
