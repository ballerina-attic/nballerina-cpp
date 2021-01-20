// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

int _bal_result = 0;
public function bar(any l) returns int
{
    return <int>l;
}

public function main() {
  int a = 35;
  _bal_result = bar(a);
}
// CHECK: RETVAL
// CHECK-SAME: 35
