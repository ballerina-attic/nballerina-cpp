// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 40;
    any b = a;
    int c = <int>b;
    print_string("RESULT=");
    print_integer(c);
}
// CHECK: RESULT=40
