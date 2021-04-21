// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function main() {
    string[] arr = [];
    arr[100] = "end";
    print_string("Result=");
    print_string(arr[50]);
}
// CHECK: Result=
