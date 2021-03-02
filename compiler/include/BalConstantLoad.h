#ifndef __BALCONSTANTLOAD__H__
#define __BALCONSTANTLOAD__H__

#include "BalNonTerminatorInsn.h"
#include "BalType.h"
#include <string>

namespace nballerina {

// TODO Convert to template class
class ConstantLoadInsn : public NonTerminatorInsn {
private:
  TypeTag typeTag;
  union value {
    int intValue;
    double floatValue;
    bool boolValue;
    std::string *strValue;
    value() {}
    value(int x) : intValue(x) {}
    value(float x) : floatValue(x) {}
    value(bool x) : boolValue(x) {}
    value(std::string *x) : strValue(x) {}
  } val;

public:
  ConstantLoadInsn() = delete;
  ConstantLoadInsn(Operand *lOp, BasicBlock *currentBB);
  ~ConstantLoadInsn() = default;

  int getIntValue();
  float getFloatValue();
  bool getBoolValue();
  std::string *getStringValue();
  TypeTag getTypeTag();

  void setIntValue(int intVal, TypeTag TypeTag);
  void setFloatValue(float floatVal, TypeTag TypeTag);
  void setBoolValue(bool boolVal, TypeTag TypeTag);
  void setStringValue(std::string *str, TypeTag TypeTag);
  void setTypeTagNil(TypeTag TypeTag);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONSTANTLOAD__H__