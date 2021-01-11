// RUN: ./../../../../../BIR2llvmIR/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = 10;
    int b = 5;
    if (a != b) {
      _bal_result = 54;
    }
}
// CHECK: RETVAL
// CHECK-SAME: 54

