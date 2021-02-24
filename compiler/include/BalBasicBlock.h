#ifndef __BALBASICBLOCK__H__
#define __BALBASICBLOCK__H__

#include "Debuggable.h"
#include "PackageNode.h"
#include "Translatable.h"
#include <string>
#include <vector>

namespace nballerina {

// forward declare
class BIRFunction;
class TerminatorInsn;
class NonTerminatorInsn;

class BIRBasicBlock : public PackageNode,
                      public Debuggable,
                      public Translatable {
private:
  std::string id;
  std::vector<NonTerminatorInsn *> instructions;
  TerminatorInsn *terminator;
  LLVMBuilderRef bRef;
  BIRFunction *bFunc;
  BIRBasicBlock *nextBB;
  LLVMBasicBlockRef bbRefObj;

public:
  BIRBasicBlock() = default;
  BIRBasicBlock(std::string id);
  ~BIRBasicBlock() = default;
  BIRBasicBlock(Location *pos, std::string id);

  std::string getId() { return id; }
  TerminatorInsn *getTerminatorInsn() { return terminator; }
  LLVMBuilderRef getLLVMBuilderRef() { return bRef; }
  BIRFunction *getBIRFunction() { return bFunc; }
  BIRBasicBlock *getNextBB() { return nextBB; }
  std::vector<NonTerminatorInsn *> getNonTerminatorInsn() {
    return instructions;
  }
  NonTerminatorInsn *getInsn(int i) { return instructions[i]; }
  size_t numInsns() { return instructions.size(); }
  LLVMBasicBlockRef getLLVMBBRef() { return bbRefObj; }

  void setId(std::string newId) { id = newId; }
  void setTerminatorInsn(TerminatorInsn *insn) { terminator = insn; }
  void setLLVMBuilderRef(LLVMBuilderRef buildRef) { bRef = buildRef; }
  void setBIRFunction(BIRFunction *func) { bFunc = func; }
  void setNextBB(BIRBasicBlock *bb) { nextBB = bb; }
  void setLLVMBBRef(LLVMBasicBlockRef bbRef) { bbRefObj = bbRef; }
  void addNonTermInsn(NonTerminatorInsn *insn) { instructions.push_back(insn); }

  void translate(LLVMModuleRef &modRef) final;
};
} // namespace nballerina

#endif //!__BALBASICBLOCK__H__