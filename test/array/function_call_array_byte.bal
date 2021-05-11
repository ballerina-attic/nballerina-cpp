// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_byte(byte val) = external;

public function test1(byte[] a) returns byte {
    byte c = a[0];
    return c;
}

public function test2(byte[] a) returns byte {
    byte c = a[1];
    return c;
}

public function test3(byte[] a) returns byte {
    byte c = a[2];
    return c;
}

public function main() {
    byte[] arr1 = [5];
    arr1[0] = 1;
    arr1[1] = 5;
    arr1[3] = 10;
    print_string("RESULT=");
    print_byte(test1(arr1));
    print_string("RESULT=");
    print_byte(test2(arr1));
    print_string("RESULT=");
    print_byte(test3(arr1));
}
// CHECK: RESULT=1
// CHECK: RESULT=5
// CHECK: RESULT=0
