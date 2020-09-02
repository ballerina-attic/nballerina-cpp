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
  LLVMBasicBlockRef bbRef = LLVMAppendBasicBlock(BFunc->getNewFunctionRef(),
						 (id).c_str());
  LLVMPositionBuilderAtEnd(BRef, bbRef);
  
  for(int i=0; i < instructions.size(); i++)
  {
    NonTerminatorInsn *insn = instructions[i];
    insn->setFunction(BFunc);
    switch (insn->getInstKind()) {
      case INS_KIND_MOVE:
      {
        MoveInsn *moveInsn = static_cast<MoveInsn*>(insn);
        moveInsn->translate(modRef);
      }

      case BINARY_ADD:
      case BINARY_SUB:
      case BINARY_MUL:
      case BINARY_DIV:
      case BINARY_EQUAL:
      {
	BinaryOpInsn *binOpInsn = static_cast<BinaryOpInsn*>(insn);
	binOpInsn->translate(modRef);
      }

      case INS_KIND_CONST_LOAD: 
      {
	ConstantLoadInsn *constL = static_cast<ConstantLoadInsn*>(insn);
	constL->translate(modRef);
      }
    } 
  }
}
