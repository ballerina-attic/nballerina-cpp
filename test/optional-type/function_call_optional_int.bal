// RUN: "%testRunScript" %s %nballerinacc "%java_path" "%target_variant" "%skip_bir_gen" | filecheck %s

public function print_string(string val) = external;

public function print_integer(int val) = external;

function foo(int? i) returns int {
    return <int>i;
}

public function main() {
    print_string("RESULT=");
    print_integer(foo(32));
}
// CHECK: RESULT=32
