// RUN: ./../../../../BIR2llvmIR/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = 58;
    _bal_result = a;
}
// CHECK: RETVAL
// CHECK-SAME: 58
