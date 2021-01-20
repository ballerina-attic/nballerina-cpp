// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

public function bar(any z) returns float
{
    float fl = <float>z;
    return fl;
}

public function main() {
  float b = 10.5;
  float c = bar(b);
}

// CHECK: RETVAL
// CHECK-SAME: 40
