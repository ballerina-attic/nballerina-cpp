// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function printu32(int val) = external;

any a = 23;

public function main() {
    a = 23;    // TODO remove ressaignment when global init is fixed
    int x = <int>a;
    print_string("RESULT=");
    printu32(x);
}
// CHECK: RESULT=23
