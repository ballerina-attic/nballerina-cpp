#include "BIR.h"

AbstractInsn::AbstractInsn() {}

AbstractInsn::AbstractInsn(Location *pos, InstructionKind kindP, Operand *lOp):
				BIRNode(pos), kind(kindP), lhsOp(lOp){}

AbstractInsn::~AbstractInsn() {}

void AbstractInsn::translate(LLVMModuleRef &modRef) {}
