// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int[]? arrOrNil = [23];
    int[] arr = <int[]>arrOrNil;
    arr[0] = 32;
    print_string("RESULT=");
    print_integer(arr[0]);
}
// CHECK: RESULT=32
