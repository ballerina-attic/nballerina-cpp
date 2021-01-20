// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    boolean e = false;
    boolean f = true;
    if (e) {
      _bal_result = 10;
    }
    if (f) {
      _bal_result = 20;
    }
}
// CHECK: RETVAL
// CHECK-SAME: 20
