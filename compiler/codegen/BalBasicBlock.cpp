#include "BalBasicBlock.h"
#include "BalFunction.h"
#include "BalNonTerminatorInsn.h"
#include "BalTerminatorInsn.h"

namespace nballerina {

BasicBlock::BasicBlock(std::string pid, Function *parentFunc)
    : id(pid), parentFunction(parentFunc) {}

std::string BasicBlock::getId() { return id; }
TerminatorInsn *BasicBlock::getTerminatorInsn() { return terminator; }
LLVMBuilderRef BasicBlock::getLLVMBuilderRef() { return bRef; }
Function *BasicBlock::getFunction() { return parentFunction; }
BasicBlock *BasicBlock::getNextBB() { return nextBB; }
std::vector<NonTerminatorInsn *> BasicBlock::getNonTerminatorInsn() {
  return instructions;
}
NonTerminatorInsn *BasicBlock::getInsn(int i) { return instructions[i]; }
size_t BasicBlock::numInsns() { return instructions.size(); }
LLVMBasicBlockRef BasicBlock::getLLVMBBRef() { return bbRefObj; }
Package *BasicBlock::getPackage() { return parentFunction->getPackage(); }

void BasicBlock::setId(std::string newId) { id = newId; }
void BasicBlock::setTerminatorInsn(TerminatorInsn *insn) { terminator = insn; }
void BasicBlock::setLLVMBuilderRef(LLVMBuilderRef buildRef) { bRef = buildRef; }
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
