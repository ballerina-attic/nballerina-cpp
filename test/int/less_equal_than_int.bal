// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function main() {
    int a = 25;
    int b = 42;
    print_string("RESULT=");
    if (a <= b) {
        print_integer(a);
    }
    else {
        print_integer(b);
    }
}
// CHECK: RESULT=25
