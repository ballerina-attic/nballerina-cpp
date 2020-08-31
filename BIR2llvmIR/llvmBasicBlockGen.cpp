#include "BIR.h"

BasicBlock1::BasicBlock1() {}

BasicBlock1::BasicBlock1(string pid) {
}

BasicBlock1::BasicBlock1(LLVMBuilderRef buildRef, BIRFunction *birFunc, BasicBlock1 *bb1)
{
  BRef       = buildRef;
  BFunc      = birFunc;
  bb         = bb1;
}
BasicBlock1::~BasicBlock1() {}

void BasicBlock1::translate(LLVMModuleRef modRef)
{
  LLVMBasicBlockRef bbRef = LLVMAppendBasicBlock(BFunc->getnewFunctionRef(), (bb->id).c_str());
  LLVMPositionBuilderAtEnd(BRef, bbRef);
  
  for(int i=0; i < instructions.size(); i++)
  {
    NonTerminatorInsn insn = instructions[i];
    switch (insn.getInstKind()) {
      case INS_KIND_MOVE:
      {
        //Move moveInsn = static_cast<Move>insn;
	Move *moveInsn = new Move(BFunc);
        moveInsn->translate();
      }

      case INS_KIND_CONST_LOAD: 
      {
	
      }
    } 
  }
}
