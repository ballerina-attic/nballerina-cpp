// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function main() {
    string[] arr = [];
    arr[100] = "end";
    print_string("Result=");
    print_string(arr[50]);
}
// CHECK: Result=
