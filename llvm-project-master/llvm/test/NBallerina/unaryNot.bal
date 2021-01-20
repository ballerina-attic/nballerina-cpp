// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    boolean e = false;
    boolean f = false;
    e = !f;
    if (e) {
      _bal_result = 0;
    }
    else {
     _bal_result = 1;
    }
}
// CHECK: RETVAL
// CHECK-SAME: 0
