// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function main() {
    int a = 10;
    int b = 5;
    int c = 0;
    int i = 0;
    boolean e = false;
    boolean f = false;
    c  = a + b;
    c = a - b;
    c = a * b;
    c = a / b;
    c = a % b;
    b = -a;
    e = !f;
    b = ~a;
    if (a == 10) {
      c = b;
    }
    if (a < b) {
      e = false;
    }
    if (a > b) {
      f = true;
    }
    if ( a <= b) {
      e = true;
    }
    if (a >= b) {
      f = false;
    }
    else {
      f = true;
    }
    while (i < 10) {
      i = i + 1;
    }
    _bal_result = a;
}

// CHECK: RETVAL
// CHECK-SAME: 10
