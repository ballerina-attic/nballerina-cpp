// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function test(int[] a) returns int {
    int c = a[0] + a[1] + a[3];
    return c;
}
public function main() {
    int[] arr1 = [5];
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;
    print_string("RESULT=");
    printu32(test(arr1));
}

// CHECK: RESULT=16
