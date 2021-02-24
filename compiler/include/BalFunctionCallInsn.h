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
  ~FunctionCallInsn() = default;

  void setIsVirtual(bool funcVirtual);
  void setFunctionName(std::string funcName);
  void setArgCount(int argNumber);
  void setArgumentsList(std::vector<Operand *> fnArgs);
  void addArgumentToList(Operand *arg);
  void setRestParam(Param *rParam);

  bool getIsVirtual();
  std::string getFunctionName();
  int getArgCount();
  Operand *getArgumentFromList(int i);
  std::vector<Operand *> getArgumentsList();
  Param *getRestParam();

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALFUNCTIONCALLINSN__H__