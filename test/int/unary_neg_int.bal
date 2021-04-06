// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i32(int val) = external;

public function main() {
    int a = -3;
    int b = 0;
    b = ~a;
    print_string("RESULT=");
    print_i32(b);
    int c = 10;
    int d = 0;
    d = ~c;
    print_string("RESULT=");
    print_i32(d);
}
// CHECK: RESULT=2
// CHECK: RESULT=-11
