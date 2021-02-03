#include "BIR.h"

BIRBasicBlock::BIRBasicBlock() {}

BIRBasicBlock::BIRBasicBlock(string pid) : id(pid) {}

BIRBasicBlock::BIRBasicBlock(Location *loc, string pid)
    : BIRNode(loc), id(pid) {}

BIRBasicBlock::~BIRBasicBlock() {}

void BIRBasicBlock::translate(LLVMModuleRef &modRef) {
  for (unsigned int i = 0; i < instructions.size(); i++) {
    NonTerminatorInsn *insn = instructions[i];

    insn->setFunction(bFunc);
    insn->setCurrentBB(this);
    insn->setPkgAddress(getPkgAddress());
    insn->translate(modRef);
  }

  if (terminator) {
    terminator->setFunction(bFunc);
    terminator->setCurrentBB(this);
    terminator->setPkgAddress(getPkgAddress());
    terminator->translate(modRef);
  }
}
