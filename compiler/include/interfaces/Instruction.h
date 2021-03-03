#ifndef __BALABSTRACTINSN__H__
#define __BALABSTRACTINSN__H__

#include "interfaces/Debuggable.h"
#include "interfaces/PackageNode.h"

namespace nballerina {

// Forward Declaration
class Operand;
class Function;
class BasicBlock;
class Package;

enum InstructionKind {
  INSTRUCTION_KIND_GOTO = 1,
  INSTRUCTION_KIND_CALL = 2,
  INSTRUCTION_KIND_CONDITIONAL_BRANCH = 3,
  INSTRUCTION_KIND_RETURN = 4,
  INSTRUCTION_KIND_BINARY_REF_EQUAL,
  INSTRUCTION_KIND_BINARY_REF_NOT_EQUAL,
  INSTRUCTION_KIND_BINARY_CLOSED_RANGE,
  INSTRUCTION_KIND_BINARY_HALF_OPEN_RANGE,
  INSTRUCTION_KIND_BINARY_ANNOT_ACCESS,
  INSTRUCTION_KIND_BINARY_BITWISE_AND,
  INSTRUCTION_KIND_BINARY_BITWISE_OR,
  INSTRUCTION_KIND_BINARY_BITWISE_LEFT_SHIFT,
  INSTRUCTION_KIND_MOVE = 20,
  INSTRUCTION_KIND_CONST_LOAD = 21,
  INSTRUCTION_KIND_NEW_STRUCTURE,
  INSTRUCTION_KIND_MAP_STORE = 23,
  INSTRUCTION_KIND_NEW_ARRAY = 25,
  INSTRUCTION_KIND_ARRAY_STORE = 26,
  INSTRUCTION_KIND_ARRAY_LOAD = 27,
  INSTRUCTION_KIND_TYPE_CAST = 29,
  INSTRUCTION_KIND_TYPE_TEST = 31,
  INSTRUCTION_KIND_NEW_TYPEDESC = 52,
  INSTRUCTION_KIND_BINARY_ADD = 61,
  INSTRUCTION_KIND_BINARY_SUB,
  INSTRUCTION_KIND_BINARY_MUL,
  INSTRUCTION_KIND_BINARY_DIV,
  INSTRUCTION_KIND_BINARY_MOD,
  INSTRUCTION_KIND_BINARY_EQUAL = 66,
  INSTRUCTION_KIND_BINARY_NOT_EQUAL = 67,
  INSTRUCTION_KIND_BINARY_GREATER_THAN = 68,
  INSTRUCTION_KIND_BINARY_GREATER_EQUAL = 69,
  INSTRUCTION_KIND_BINARY_LESS_THAN = 70,
  INSTRUCTION_KIND_BINARY_LESS_EQUAL = 71,
  INSTRUCTION_KIND_UNARY_NOT = 81,
  INSTRUCTION_KIND_UNARY_NEG,
  INSTRUCTION_KIND_BINARY_BITWISE_XOR = 85,
  INSTRUCTION_KIND_BINARY_BITWISE_UNSIGNED_RIGHT_SHIFT
};

class Instruction : public Debuggable, public PackageNode {
private:
  Operand *lhsOp;
  BasicBlock *parentBB;

protected:
  Operand *getLHS();
  Function *getFunction();

public:
  Instruction() = delete;
  Instruction(Operand *lOp, BasicBlock *parentBB);
  virtual ~Instruction() = default;
  Package *getPackage() final;
};

} // namespace nballerina

#endif //!__BALABSTRACTINSN__H__
