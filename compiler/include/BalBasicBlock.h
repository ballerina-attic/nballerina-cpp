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
    Function *parentFunction;
    TerminatorInsn *terminator;
    BasicBlock *nextBB;
    std::vector<NonTerminatorInsn *> instructions;
    LLVMBasicBlockRef bbRefObj;

  public:
    BasicBlock() = delete;
    BasicBlock(std::string id, Function *parentFunc);
    ~BasicBlock() = default;

    std::string &getId();
    TerminatorInsn *getTerminatorInsn();
    Function *getFunction();
    BasicBlock *getNextBB();
    LLVMBasicBlockRef getLLVMBBRef();
    Package *getPackage() final;

    void setNextBB(BasicBlock *bb);
    void setTerminatorInsn(TerminatorInsn *insn);
    void addNonTermInsn(NonTerminatorInsn *insn);
    void setLLVMBBRef(LLVMBasicBlockRef bbRef);

    void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALBASICBLOCK__H__
