// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = 10;
    int b = 5;
    int c = 8;
    boolean e = false;
    boolean f = true;
    if (a > b) {
      if (f) {
        _bal_result = a;
      }
      else {
	_bal_result = b;
      }
    }
    else {
      _bal_result = c;
    }
}
// CHECK: RETVAL
// CHECK-SAME: 10
