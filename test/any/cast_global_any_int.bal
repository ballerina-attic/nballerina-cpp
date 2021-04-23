// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

any a = 23;

public function main() {
    a = 23;    // TODO remove ressaignment when global init is fixed
    int x = <int>a;
    print_string("RESULT=");
    print_integer(x);
}
// CHECK: RESULT=23
