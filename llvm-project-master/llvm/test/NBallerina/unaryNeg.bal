// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = -3;
    int b = 0;
    b = ~a;
    _bal_result = b;
}
// CHECK: RETVAL
// CHECK-SAME: 2
