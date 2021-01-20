// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

boolean _bal_result = false;
public function bar(any z) returns boolean
{
    return <boolean>z;
}

public function main() {
  boolean b = true;
  _bal_result = bar(b);
}

// CHECK: RETVAL
// CHECK-SAME: 1
