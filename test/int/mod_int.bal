// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function main() {
    int a = 3;
    int b = 4;
    int c = 0;
    c = a % b;
    print_string("RESULT=");
    print_i64(c);
}
// CHECK: RESULT=3
