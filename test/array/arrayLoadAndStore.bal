// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;
public function main() {
  int[] arr1 = [5];
  arr1[0] = 1;
  arr1[1] = 5;
  arr1[3] = 10;
  _bal_result = arr1[0] + arr1[1] + arr1[3];
}

// CHECK: RETVAL
// CHECK-SAME: 16
