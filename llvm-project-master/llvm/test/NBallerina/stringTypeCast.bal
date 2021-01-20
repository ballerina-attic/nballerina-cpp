// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

public function bar(any z) returns string
{
    string strl = <string>z;
    return strl;
}

public function main() {
  string b = "Hello Wolrd";
  string c = bar(b);
}

// CHECK: RETVAL
// CHECK-SAME: 40
