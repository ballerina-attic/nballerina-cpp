// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

public function main() {
    int[] arr = [];
    arr[100] = 10;
    print_string("Result=");
    printu32(arr[50]);
}
// CHECK: Result=0
