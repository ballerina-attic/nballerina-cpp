#include "BIR.h"

BinaryOpInsn::BinaryOpInsn() {}

BinaryOpInsn::BinaryOpInsn(Location *pos, InstructionKind kind, Operand *lOp,
                           Operand *rOp1, Operand *rOp2)
    : NonTerminatorInsn(pos, kind, lOp), rhsOp1(rOp1), rhsOp2(rOp2) {}

BinaryOpInsn::~BinaryOpInsn() {}

void BinaryOpInsn::translate(LLVMModuleRef &modRef) {
  BIRFunction *funcObj = getFunction();
  LLVMBuilderRef builder;
  string lhsName, lhstmpName;
  LLVMValueRef lhsRef;
  LLVMValueRef rhsOp1ref;
  LLVMValueRef rhsOp2ref;
  LLVMValueRef ifReturn;
  if (funcObj && getLhsOperand() && getLhsOperand()->getVarDecl()) {
    builder = funcObj->getLLVMBuilder();
    lhsName = getLhsOperand()->name();
    lhstmpName = lhsName + "_temp";
    lhsRef = funcObj->getLocalVarRefUsingId(lhsName);
    if (!lhsRef)
      lhsRef = getPkgAddress()->getGlobalVarRefUsingId(lhsName);
  }

  if (funcObj && rhsOp1 && rhsOp2) {
    rhsOp1ref = funcObj->getLocalToTempVar(rhsOp1);
    rhsOp2ref = funcObj->getLocalToTempVar(rhsOp2);
  }

  if (rhsOp1ref && rhsOp2ref && builder) {
    switch (getInstKind()) {
    case INSTRUCTION_KIND_BINARY_ADD: {
      ifReturn =
          LLVMBuildAdd(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      if (ifReturn) {
        LLVMBuildStore(builder, ifReturn, lhsRef);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_SUB: {
      ifReturn =
          LLVMBuildSub(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      if (ifReturn) {
        LLVMBuildStore(builder, ifReturn, lhsRef);
      }

      break;
    }
    case INSTRUCTION_KIND_BINARY_MUL: {
      ifReturn =
          LLVMBuildMul(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      if (ifReturn) {
        LLVMBuildStore(builder, ifReturn, lhsRef);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_DIV: {
      ifReturn =
          LLVMBuildUDiv(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      if (ifReturn) {
        LLVMBuildStore(builder, ifReturn, lhsRef);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_MOD: {
      ifReturn =
          LLVMBuildURem(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      if (ifReturn) {
        LLVMBuildStore(builder, ifReturn, lhsRef);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_THAN: {
      ifReturn = LLVMBuildICmp(builder, LLVMIntUGT, rhsOp1ref, rhsOp2ref,
                               lhstmpName.c_str());
      if (ifReturn && funcObj) {
        funcObj->addNewbranchComparison(lhsName, ifReturn);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_GREATER_EQUAL: {
      ifReturn = LLVMBuildICmp(builder, LLVMIntUGE, rhsOp1ref, rhsOp2ref,
                               lhstmpName.c_str());
      if (ifReturn && funcObj) {
        funcObj->addNewbranchComparison(lhsName, ifReturn);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_THAN: {
      ifReturn = LLVMBuildICmp(builder, LLVMIntULT, rhsOp1ref, rhsOp2ref,
                               lhstmpName.c_str());
      if (ifReturn && funcObj) {
        funcObj->addNewbranchComparison(lhsName, ifReturn);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_LESS_EQUAL: {
      ifReturn = LLVMBuildICmp(builder, LLVMIntULE, rhsOp1ref, rhsOp2ref,
                               lhstmpName.c_str());
      if (ifReturn && funcObj) {
        funcObj->addNewbranchComparison(lhsName, ifReturn);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_EQUAL: {
      ifReturn = LLVMBuildICmp(builder, LLVMIntEQ, rhsOp1ref, rhsOp2ref,
                               lhstmpName.c_str());
      if (ifReturn && funcObj) {
        funcObj->addNewbranchComparison(lhsName, ifReturn);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_NOT_EQUAL: {
      ifReturn = LLVMBuildICmp(builder, LLVMIntNE, rhsOp1ref, rhsOp2ref,
                               lhstmpName.c_str());
      if (ifReturn && funcObj) {
        funcObj->addNewbranchComparison(lhsName, ifReturn);
      }
      break;
    }
    case INSTRUCTION_KIND_BINARY_BITWISE_XOR: {
      ifReturn =
          LLVMBuildXor(builder, rhsOp1ref, rhsOp2ref, lhstmpName.c_str());
      if (ifReturn) {
        LLVMBuildStore(builder, ifReturn, lhsRef);
      }
      break;
    }
    default:
      break;
    }
  }
}
