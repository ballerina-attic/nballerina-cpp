// RUN: JAVA_HOME=%JAVA_HOME WORK_PATH=%WORK_PATH %WORK_PATH/testRunScript.sh %s -o - | FileCheck %s

public function main() {
  string str = "Hello World!";
}
// CHECK: RETVAL
// CHECK-SAME:40
