// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

public function bar(int z) returns int {
    return z + 10;
}

public function main() {
    int a = 5;
    print_string("RESULT=");
    print_i64(bar(a));
}
// CHECK: RESULT=15
