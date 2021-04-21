// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

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
    print_integer(test(arr1));
}

// CHECK: RESULT=16
