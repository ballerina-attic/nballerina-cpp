#ifndef BIR2LLVM_IR_CONVERTOR_H
#define BIR2LLVM_IR_CONVERTOR_H

#include <string>
#include <vector>
#include "BIR.h"
//#include "BIRReader.h"

using namespace std;

class BIR2LLVMIRConvertor {
  private:
    BIRPackage    package;

  public:
    BIR2LLVMIRConvertor();
    ~BIR2LLVMIRConvertor();

    void translate();
};

#endif  // BIR2LLVM_IR_CONVERTOR_H
