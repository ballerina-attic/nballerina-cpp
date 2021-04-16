// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int[] arr = [];
    arr[100] = 10;
    print_string("Result=");
    print_integer(arr[50]);
}
// CHECK: Result=0
