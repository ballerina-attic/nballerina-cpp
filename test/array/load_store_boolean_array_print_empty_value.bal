// RUN: "%testRunScript" %s %nballerinacc "%java_path" | filecheck %s

public function print_string(string val) = external;

public function print_boolean(boolean val) = external;

public function main() {
    boolean[] arr = [];
    arr[100] = true;
    print_string("Result=");
    print_boolean(arr[50]);
}
// CHECK: Result=false
