#include "BalBasicBlock.h"
#include "BalNonTerminatorInsn.h"
#include "BalTerminatorInsn.h"

namespace nballerina {

BasicBlock::BasicBlock(std::string pid) : id(pid) {}

BasicBlock::BasicBlock(Location *loc, std::string pid) : id(pid) {
  setLocation(loc);
}

std::string BasicBlock::getId() { return id; }
TerminatorInsn *BasicBlock::getTerminatorInsn() { return terminator; }
LLVMBuilderRef BasicBlock::getLLVMBuilderRef() { return bRef; }
Function *BasicBlock::getFunction() { return bFunc; }
BasicBlock *BasicBlock::getNextBB() { return nextBB; }
std::vector<NonTerminatorInsn *> BasicBlock::getNonTerminatorInsn() {
  return instructions;
}
NonTerminatorInsn *BasicBlock::getInsn(int i) { return instructions[i]; }
size_t BasicBlock::numInsns() { return instructions.size(); }
LLVMBasicBlockRef BasicBlock::getLLVMBBRef() { return bbRefObj; }

void BasicBlock::setId(std::string newId) { id = newId; }
void BasicBlock::setTerminatorInsn(TerminatorInsn *insn) { terminator = insn; }
void BasicBlock::setLLVMBuilderRef(LLVMBuilderRef buildRef) { bRef = buildRef; }
void BasicBlock::setFunction(Function *func) { bFunc = func; }
void BasicBlock::setNextBB(BasicBlock *bb) { nextBB = bb; }
void BasicBlock::setLLVMBBRef(LLVMBasicBlockRef bbRef) { bbRefObj = bbRef; }
void BasicBlock::addNonTermInsn(NonTerminatorInsn *insn) {
  instructions.push_back(insn);
}

void BasicBlock::translate(LLVMModuleRef &modRef) {
  for (const auto &instruction : instructions) {
    instruction->translate(modRef);
  }

  if (terminator) {
    terminator->translate(modRef);
  }
}

} // namespace nballerina
