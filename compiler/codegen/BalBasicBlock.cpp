#include "BalBasicBlock.h"
#include "BalNonTerminatorInsn.h"
#include "BalTerminatorInsn.h"

using namespace std;

namespace nballerina {

BIRBasicBlock::BIRBasicBlock(string pid) : id(pid) {}

BIRBasicBlock::BIRBasicBlock(Location *loc, string pid) : id(pid) {
  setLocation(loc);
}

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

} // namespace nballerina
