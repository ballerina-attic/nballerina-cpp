// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int i = 0;
    while (i < 5) {
      i = i + 2;
    }
    _bal_result = i;
}
// CHECK: RETVAL
// CHECK-SAME: 6
