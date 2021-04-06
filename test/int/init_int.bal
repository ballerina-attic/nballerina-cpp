// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i32(int val) = external;

public function main() {
    int a = 58;
    print_string("RESULT=");
    print_i32(a);
    int b = -42;
    print_string("RESULT=");
    print_i32(b);
}
// CHECK: RESULT=58
// CHECK: RESULT=-42
