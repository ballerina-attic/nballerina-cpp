// RUN: ./../../../../BIR2llvmIR/testRunScript.sh %s -o - | FileCheck %s

public function main() {
}
// CHECK: RETVAL
// CHECK-SAME: 128
