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
BinaryOpInsn::BinaryOpInsn(Operand *lOp, BasicBlock *currentBB, Operand *rOp1,
                           Operand *rOp2)
    : NonTerminatorInsn(lOp, currentBB), rhsOp1(rOp1), rhsOp2(rOp2) {}

void BinaryOpInsn::setInstKind(InstructionKind _kind) { kind = _kind; }

void BinaryOpInsn::translate(__attribute__((unused)) LLVMModuleRef &modRef) {

  Function *funcObj = getFunction();
  LLVMBuilderRef builder = funcObj->getLLVMBuilder();

  string lhsName = getLhsOperand()->getName();
  string lhstmpName = lhsName + "_temp";
  LLVMValueRef lhsRef = funcObj->getLocalOrGlobalLLVMValue(getLhsOperand());
  LLVMValueRef rhsOp1ref = funcObj->getLocalToTempVar(rhsOp1);
  LLVMValueRef rhsOp2ref = funcObj->getLocalToTempVar(rhsOp2);

  LLVMValueRef ifReturn;
  switch (kind) {
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