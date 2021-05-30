// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_byte(byte val) = external;

public function main() {
    byte[] arr = [];
    arr[100] = 10;
    print_string("Result=");
    print_byte(arr[50]);
}
// CHECK: Result=0
