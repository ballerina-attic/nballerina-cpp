#include "BalBinaryOpInsn.h"
#include "BalFunction.h"
#include "BalOperand.h"
#include "BalPackage.h"
#include <llvm-c/Core.h>
#include <string>

#ifndef unix
#define __attribute__(unused)
#endif

using namespace std;

namespace nballerina {
BinaryOpInsn::BinaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *rOp1, Operand *rOp2)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp1(rOp1), rhsOp2(rOp2) {}

Operand *BinaryOpInsn::getRhsOp1() { return rhsOp1; }
Operand *BinaryOpInsn::getRhsOp2() { return rhsOp2; }
void BinaryOpInsn::setRhsOp1(Operand *op) { rhsOp1 = op; }
void BinaryOpInsn::setRhsOp2(Operand *op) { rhsOp2 = op; }

void BinaryOpInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

  Function *funcObj = getFunction();
  assert(funcObj);
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();
  assert(builder);

  assert(getLhsOperand() && getLhsOperand()->getVarDecl());
  string lhsName = getLhsOperand()->getName();
  string lhstmpName = lhsName + "_temp";
  LLVMValueRef lhsRef = funcObj->getLocalVarRefUsingId(lhsName);
  if (!lhsRef)
    lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsName);

  assert(rhsOp1 && rhsOp2);
  LLVMValueRef rhsOp1ref = funcObj->getLocalToTempVar(rhsOp1);
  LLVMValueRef rhsOp2ref = funcObj->getLocalToTempVar(rhsOp2);

  if (!(rhsOp1ref && rhsOp2ref && builder))
    return;

  LLVMValueRef ifReturn;
  switch (getInstKind()) {
  case INSTRUCTION_KIND_BINARY_ADD: {
    ifReturn = LLVMBuildAdd(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
    if (ifReturn) {
      LLVMBuildStore(builder, ifReturn, lhsRef);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_SUB: {
    ifReturn = LLVMBuildSub(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
    if (ifReturn) {
      LLVMBuildStore(builder, ifReturn, lhsRef);
    }

    break;
  }
  case INSTRUCTION_KIND_BINARY_MUL: {
    ifReturn = LLVMBuildMul(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
    if (ifReturn) {
      LLVMBuildStore(builder, ifReturn, lhsRef);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_DIV: {
    ifReturn = LLVMBuildUDiv(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
    if (ifReturn) {
      LLVMBuildStore(builder, ifReturn, lhsRef);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_MOD: {
    ifReturn = LLVMBuildURem(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
    if (ifReturn) {
      LLVMBuildStore(builder, ifReturn, lhsRef);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_GREATER_THAN: {
    ifReturn = LLVMBuildICmp(builder, LLVMIntUGT, rhsOp1ref, rhsOp2ref,
                             lhstmpName.c_str());
    if (ifReturn) {
      funcObj->addNewbranchComparison(lhsName, ifReturn);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_GREATER_EQUAL: {
    ifReturn = LLVMBuildICmp(builder, LLVMIntUGE, rhsOp1ref, rhsOp2ref,
                             lhstmpName.c_str());
    if (ifReturn) {
      funcObj->addNewbranchComparison(lhsName, ifReturn);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_LESS_THAN: {
    ifReturn = LLVMBuildICmp(builder, LLVMIntULT, rhsOp1ref, rhsOp2ref,
                             lhstmpName.c_str());
    if (ifReturn) {
      funcObj->addNewbranchComparison(lhsName, ifReturn);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_LESS_EQUAL: {
    ifReturn = LLVMBuildICmp(builder, LLVMIntULE, rhsOp1ref, rhsOp2ref,
                             lhstmpName.c_str());
    if (ifReturn) {
      funcObj->addNewbranchComparison(lhsName, ifReturn);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_EQUAL: {
    ifReturn = LLVMBuildICmp(builder, LLVMIntEQ, rhsOp1ref, rhsOp2ref,
                             lhstmpName.c_str());
    if (ifReturn) {
      funcObj->addNewbranchComparison(lhsName, ifReturn);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_NOT_EQUAL: {
    ifReturn = LLVMBuildICmp(builder, LLVMIntNE, rhsOp1ref, rhsOp2ref,
                             lhstmpName.c_str());
    if (ifReturn) {
      funcObj->addNewbranchComparison(lhsName, ifReturn);
    }
    break;
  }
  case INSTRUCTION_KIND_BINARY_BITWISE_XOR: {
    ifReturn = LLVMBuildXor(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
    if (ifReturn) {
      LLVMBuildStore(builder, ifReturn, lhsRef);
    }
    break;
  }
  default:
    break;
  }
}
} // namespace nballerina