#ifndef __BALBASICBLOCK__H__
#define __BALBASICBLOCK__H__

#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"
#include "interfaces/Translatable.h"
#include <string>
#include <vector>

namespace nballerina {

// Forward Declaration
class Function;
class TerminatorInsn;
class NonTerminatorInsn;

class BasicBlock : public PackageNode, public Debuggable, public Translatable {
private:
  std::string id;
  std::vector<NonTerminatorInsn *> instructions;
  TerminatorInsn *terminator;
  LLVMBuilderRef bRef;
  Function *parentFunction;
  BasicBlock *nextBB;
  LLVMBasicBlockRef bbRefObj;

public:
  BasicBlock() = delete;
  BasicBlock(std::string id, Function *parentFunc);
  ~BasicBlock() = default;

  std::string getId();
  TerminatorInsn *getTerminatorInsn();
  LLVMBuilderRef getLLVMBuilderRef();
  Function *getFunction();
  BasicBlock *getNextBB();
  std::vector<NonTerminatorInsn *> getNonTerminatorInsn();
  NonTerminatorInsn *getInsn(int i);
  size_t numInsns();
  LLVMBasicBlockRef getLLVMBBRef();
  Package *getPackage() final;

  void setId(std::string newId);
  void setTerminatorInsn(TerminatorInsn *insn);
  void setLLVMBuilderRef(LLVMBuilderRef buildRef);
  void setNextBB(BasicBlock *bb);
  void setLLVMBBRef(LLVMBasicBlockRef bbRef);
  void addNonTermInsn(NonTerminatorInsn *insn);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALBASICBLOCK__H__
