// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 58;
    print_string("RESULT=");
    print_integer(a);
    int b = -42;
    print_string("RESULT=");
    print_integer(b);
}
// CHECK: RESULT=58
// CHECK: RESULT=-42
