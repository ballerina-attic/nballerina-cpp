#include "BIR.h"

NonTerminatorInsn::NonTerminatorInsn() {}

NonTerminatorInsn::NonTerminatorInsn(Location posi, InstructionKind kindP):
	AbstractInsn(posi, kindP) {}

NonTerminatorInsn::~NonTerminatorInsn () {}

NonTerminatorInsn::NonTerminatorInsn(BIRFunction *bFunc):BFunc(bFunc) {

}

NonTerminatorInsn::NonTerminatorInsn(Operand lOp):AbstractInsn(lOp) {
}

void NonTerminatorInsn::translate(){
}

BIRFunction* NonTerminatorInsn::getFunction(){
  return BFunc;
}
