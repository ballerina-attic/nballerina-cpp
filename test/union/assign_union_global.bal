// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

int|string a = 24;

public function main() {
    a = 24;
    int x = <int>a;
    print_string("RESULT=");
    print_i64(x);
}
// CHECK: RESULT=24
