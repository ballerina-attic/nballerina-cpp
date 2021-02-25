// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
  int[] arr1 = [5];
  _bal_result = 1;
}

// CHECK: RETVAL
// CHECK-SAME: 1
