// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_byte(byte val) = external;

public function test(byte[] a) returns byte {
    byte c = a[0];
    return c;
}
public function main() {
    byte[] arr1 = [5];
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;
    print_string("RESULT=");
    print_byte(test(arr1));
}

// CHECK: RESULT=1
