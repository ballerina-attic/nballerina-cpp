// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = 3;
    int b = 4;
    int c = 0;
    c = a % b;
    _bal_result = c;
}
// CHECK: RETVAL
// CHECK-SAME: 3
