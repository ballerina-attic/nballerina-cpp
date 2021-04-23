// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function bar(int x, int y, int z) returns int {
    return x + y * z;
}

public function main() {
    int a = 5;
    int b = 10;
    print_string("RESULT=");
    print_integer(bar(a, b, a));
}
// CHECK: RESULT=55
