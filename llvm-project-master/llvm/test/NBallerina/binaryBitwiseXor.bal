// RUN: ./../../../../BIR2llvmIR/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = 0;
    int b = 1;
    int c =a^b;
    _bal_result = c;
}
// CHECK: RETVAL
// CHECK-SAME: 1
