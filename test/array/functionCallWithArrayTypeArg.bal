// RUN: JAVA_HOME=%java_path %testRunScript %s %nballerinacc | filecheck %s

int _bal_result = 0;

public function test(int[] a) returns int {
    int c = a[0] + a[1] + a[3];
    return c;
}
public function main() {
    int[] arr1 = [5];
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;
    _bal_result = test(arr1);
}

// CHECK: RETVAL=16
