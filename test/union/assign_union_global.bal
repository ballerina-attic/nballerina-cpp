// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

int|string a = 24;

public function main() {
    a = 24;
    int x = <int>a;
    print_string("RESULT=");
    print_integer(x);
}
// CHECK: RESULT=24
