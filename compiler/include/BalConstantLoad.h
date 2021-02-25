#ifndef __BALCONSTANTLOAD__H__
#define __BALCONSTANTLOAD__H__

#include "BalEnums.h"
#include "BalNonTerminatorInsn.h"
#include <string>

namespace nballerina {

// TODO Convert to template class
class ConstantLoadInsn : public NonTerminatorInsn {
private:
  TypeTagEnum typeTag;
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
  TypeTagEnum getTypeTag();

  void setIntValue(int intVal, TypeTagEnum TypeTag);
  void setFloatValue(float floatVal, TypeTagEnum TypeTag);
  void setBoolValue(bool boolVal, TypeTagEnum TypeTag);
  void setStringValue(std::string *str, TypeTagEnum TypeTag);
  void setTypeTagNil(TypeTagEnum TypeTag);

  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONSTANTLOAD__H__