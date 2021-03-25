// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function test(int[] a, int[] b) returns int {
    int c =  b[3] + a[0] + a[1];
    return c;
}
public function main() {
    int[] arr1 = [5];
    arr1[0] = 1;
    arr1[1] = 5;
    int[] arr2 = [5];
    arr2[3] = 10;
    print_string("RESULT=");
    printu32(test(arr1, arr2));
}

// CHECK: RESULT=16
