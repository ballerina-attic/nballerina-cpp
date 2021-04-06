// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

public function bar(int z) returns int {
    return z + 10;
}

public function main() {
    print_string("RESULT=");
    print_integer(bar(5));
}
// CHECK: RESULT=15
