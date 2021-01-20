// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function bar(int z) returns int
{
    return z + 10;
}

public function main() {
  int a = 5;
  _bal_result = bar(a);
}
// CHECK: RETVAL
// CHECK-SAME: 15
