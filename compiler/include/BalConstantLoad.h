#ifndef __BALCONSTANTLOAD__H__
#define __BALCONSTANTLOAD__H__

#include "BalNonTerminatorInsn.h"
#include <string>

namespace nballerina {

class ConstantLoadInsn : public NonTerminatorInsn {
private:
  enum TypeTagEnum typeTag;
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
  ConstantLoadInsn() = default;
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   int intval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   float floatval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   bool boolval);
  ConstantLoadInsn(Location *pos, InstructionKind kind, Operand *lOp,
                   std::string *strval);
  ~ConstantLoadInsn() = default;

  int getIntValue() { return val.intValue; }
  float getFloatValue() { return val.floatValue; }
  bool getBoolValue() { return val.boolValue; }
  std::string *getStringValue() { return val.strValue; }
  void setIntValue(int intVal, TypeTagEnum TypeTag) {
    val.intValue = intVal;
    typeTag = TypeTag;
  }
  void setFloatValue(float floatVal, TypeTagEnum TypeTag) {
    val.floatValue = floatVal;
    typeTag = TypeTag;
  }
  void setBoolValue(bool boolVal, TypeTagEnum TypeTag) {
    val.boolValue = boolVal;
    typeTag = TypeTag;
  }
  void setStringValue(std::string *str, TypeTagEnum TypeTag) {
    val.strValue = str;
    typeTag = TypeTag;
  }
  // With Nil Type setting only Type Tag because value will be zero with NIL
  // Type.
  void setTypeTagNil(TypeTagEnum TypeTag) { typeTag = TypeTag; }
  TypeTagEnum getTypeTag() { return typeTag; }
  void translate(LLVMModuleRef &modRef) final;
};

} // namespace nballerina

#endif //!__BALCONSTANTLOAD__H__