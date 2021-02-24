#ifndef __BALBASICBLOCK__H__
#define __BALBASICBLOCK__H__

#include "Debuggable.h"
#include "PackageNode.h"
#include "Translatable.h"
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class BIRFunction;
class TerminatorInsn;
class NonTerminatorInsn;

class BasicBlock : public PackageNode,
                      public Debuggable,
                      public Translatable {
private:
  std::string id;
  std::vector<NonTerminatorInsn *> instructions;
  TerminatorInsn *terminator;
  LLVMBuilderRef bRef;
  BIRFunction *bFunc;
  BasicBlock *nextBB;
  LLVMBasicBlockRef bbRefObj;

public:
  BasicBlock() = default;
  BasicBlock(std::string id);
  BasicBlock(Location *pos, std::string id);
  ~BasicBlock() = default;

  std::string getId();
  TerminatorInsn *getTerminatorInsn();
  LLVMBuilderRef getLLVMBuilderRef();
  BIRFunction *getBIRFunction();
  BasicBlock *getNextBB();
  std::vector<NonTerminatorInsn *> getNonTerminatorInsn();
  NonTerminatorInsn *getInsn(int i);
  size_t numInsns();
  LLVMBasicBlockRef getLLVMBBRef();

  void setId(std::string newId);
  void setTerminatorInsn(TerminatorInsn *insn);
  void setLLVMBuilderRef(LLVMBuilderRef buildRef);
  void setBIRFunction(BIRFunction *func);
  void setNextBB(BasicBlock *bb);
  void setLLVMBBRef(LLVMBasicBlockRef bbRef);
  void addNonTermInsn(NonTerminatorInsn *insn);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALBASICBLOCK__H__