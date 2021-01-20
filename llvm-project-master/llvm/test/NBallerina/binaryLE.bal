// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = 25;
    int b = 42;
    if (a <= b) {
      _bal_result = a;
    }
    else {
      _bal_result = b;
    }
}
// CHECK: RETVAL
// CHECK-SAME: 25
