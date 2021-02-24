#ifndef __BALFUNCTIONCALLINSN__H__
#define __BALFUNCTIONCALLINSN__H__

#include "BalParam.h"
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
  Param *restParam;

public:
  FunctionCallInsn() = default;
  void setIsVirtual(bool funcVirtual) { isVirtual = funcVirtual; }
  void setFunctionName(std::string funcName) { functionName = funcName; }
  void setArgCount(int argNumber) { argCount = argNumber; }
  void setArgumentsList(std::vector<Operand *> fnArgs) { argsList = fnArgs; }
  void addArgumentToList(Operand *arg) { argsList.push_back(arg); }
  void setRestParam(Param *rParam) { restParam = rParam; }

  bool getIsVirtual() { return isVirtual; }
  std::string getFunctionName() { return functionName; }
  int getArgCount() { return argCount; }
  Operand *getArgumentFromList(int i) { return argsList[i]; }
  std::vector<Operand *> getArgumentsList() { return argsList; }
  FunctionCallInsn(std::string funcName, bool funcVirtual, int argNumber,
                   Operand *lhsOp, BasicBlock *thenBB);
  Param *getRestParam() { return restParam; }
  ~FunctionCallInsn() = default;
  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALFUNCTIONCALLINSN__H__