// RUN: ./../../../../BIR2llvmIR/testRunScript.sh %s -o - | FileCheck %s

public function main() {
  string str = "Hello World!";
}
// CHECK: RETVAL
// CHECK-SAME:40
