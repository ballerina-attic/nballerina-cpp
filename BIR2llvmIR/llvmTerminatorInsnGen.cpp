#include "BIR.h"

TerminatorInsn::TerminatorInsn() {}

TerminatorInsn::TerminatorInsn(Location *pos, InstructionKind kind, Operand *lOp, 
				BIRBasicBlock *then):AbstractInsn(pos, kind,lOp),
					thenBB(then) {}

TerminatorInsn::~TerminatorInsn() {}

void TerminatorInsn::translate(LLVMModuleRef &modRef) {}
