// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_i64(int val) = external;

function foo(int? i) returns int {
    return <int>i;
}

public function main() {
    print_string("RESULT=");
    print_i64(foo(32));
}
// CHECK: RESULT=32
