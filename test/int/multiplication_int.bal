// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 10;
    int b = 5;
    print_string("RESULT=");
    print_integer(a * b);
}
// CHECK: RESULT=50
