#ifndef __BALFUNCTIONCALLINSN__H__
#define __BALFUNCTIONCALLINSN__H__

#include "BalRestParam.h"
#include "BalTerminatorInsn.h"
#include <string>
#include <vector>

namespace nballerina {

class FunctionCallInsn : public TerminatorInsn {
private:
  bool isVirtual;
  std::string functionName;
  int argCount;
  std::vector<Operand *> argsList;
  RestParam *restParam;

public:
  FunctionCallInsn() = delete;
  FunctionCallInsn(bool funcVirtual, std::string _funcName, int argNumber,
                   BasicBlock *nextBB, Operand *lhsOp,
                   std::vector<Operand *> &fnArgs, BasicBlock *currentBB);
  ~FunctionCallInsn() = default;

  std::string getFunctionName();
  int getArgCount();
  Operand *getArgumentFromList(int i);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALFUNCTIONCALLINSN__H__
